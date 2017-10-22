package main

import (
	"bytes"
	"crypto/aes"
	"crypto/cipher"
	"crypto/rand"
	"encoding/base64"
	"encoding/hex"
	"errors"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"os"

	mrand "math/rand"
)

const bsize = 16

type oracle struct {
	secret []byte
	cblock cipher.Block
}

func newOracle(key, secret []byte) (*oracle, error) {
	if len(key) != bsize {
		return nil, errors.New("key size must be 16")
	}

	cblock, err := aes.NewCipher(key)
	if err != nil {
		return nil, err
	}

	return &oracle{
		secret: secret,
		cblock: cblock,
	}, nil
}

func (o *oracle) encrypt(r io.Reader, w io.Writer) error {
	var ibuf [bsize]byte
	var obuf [bsize]byte
	var rbuf [bsize]byte

	randprefixLength := mrand.Intn(5) + 5
	rprefix := rbuf[:randprefixLength]
	_, err := io.ReadFull(rand.Reader, rprefix)
	if err != nil {
		return err
	}

	secretR := bytes.NewReader(o.secret)
	r = io.MultiReader(bytes.NewReader(rprefix), r, secretR)

	for {
		nread, err := io.ReadFull(r, ibuf[:])

		if err == io.EOF {
			return nil
		}

		if err != nil && err != io.ErrUnexpectedEOF {
			return err
		}

		if nread != bsize {
			npad := bsize - nread
			for i := 0; i < npad; i++ {
				ibuf[nread+i] = 4
			}
		}

		// log.Println("encrypt block:", hex.EncodeToString(ibuf[:]))

		o.cblock.Encrypt(obuf[:], ibuf[:])

		_, werr := w.Write(obuf[:])

		if werr != nil {
			return werr
		}

		if err == io.ErrUnexpectedEOF {
			return nil
		}
	}

}

func padbytes(in []byte, padchar byte, bsize int) []byte {
	if len(in)%bsize == 0 {
		return in
	}

	npad := bsize - len(in)%bsize

	out := make([]byte, len(in)+npad)

	copy(out, in)

	for i := len(in); i < len(in)+npad; i++ {
		out[i] = padchar
	}

	return out
}

func hexFormatCopy(w io.Writer, r io.Reader, bsize uint) (err error) {
	ibuf := make([]byte, bsize)

	n := 0
	for {
		nread, err := r.Read(ibuf[:])
		if nread > 0 {

			_, werr := fmt.Fprintf(w, "%04d: %x\n", n, ibuf)
			if werr != nil {
				return werr
			}
			n += nread
		}

		if err == io.EOF {
			return nil
		}

		if err != nil {
			return err
		}
	}
}

func test2() error {
	var key [16]byte
	_, err := rand.Read(key[:])
	if err != nil {
		return err
	}

	keyhex := hex.EncodeToString(key[:])

	fmt.Println("keyhex", keyhex)

	c, err := aes.NewCipher(key[:])
	if err != nil {
		return err
	}

	ctext := make([]byte, 16)
	ptext := padbytes([]byte("hello go AES"), 0x0, 16)
	fmt.Println("ptext", len(ptext), hex.EncodeToString(ptext))

	c.Encrypt(ctext, ptext)

	fmt.Println("ctext", hex.EncodeToString(ctext))

	dtext := make([]byte, 16)

	c.Decrypt(dtext, ctext)

	fmt.Println("d(ctext):", string(dtext))

	return nil
}

func readbase64File(filepath string) ([]byte, error) {
	f, err := os.Open(filepath)
	if err != nil {
		return nil, err
	}
	defer f.Close()

	b64d := base64.NewDecoder(base64.StdEncoding, f)
	return ioutil.ReadAll(b64d)
}

const alignMaxTries = 1000

var (
	randomPrefixPadding = make([]byte, 256)
	blankdata           = make([]byte, 256*2)
)

func init() {
	for i := range blankdata {
		blankdata[i] = 1
	}
}

func alignEncrypt(w *bytes.Buffer, o *oracle, guess []byte, bsize int) ([]byte, error) {
	if bsize > 256 {
		panic("max supported blocksize is 256")
	}

	// X = unknown target text
	// R = unknown random padding
	//
	// string to construct (assuming bsize = 4 for illustration):
	//
	// [R0 R1 RP RP] [DP DP DP DP] [DP DP DP DP] [GP GP GP X1] [X2 X3 X4]
	//
	// RP = random padding to align the random prefix
	// DP = duplicate blocks to detect alignment success
	// GP = padding attack prefix
	//
	// G = guess text = GP + X
	//
	// @returns encrypt(G)

	dp := blankdata[0 : bsize*2]
	gp := guess

	for i := 0; i < alignMaxTries; i++ {
		rpLen := mrand.Intn(bsize)
		rp := randomPrefixPadding[0:rpLen]

		content := io.MultiReader(
			bytes.NewReader(rp),
			bytes.NewReader(dp),
			bytes.NewReader(gp),
		)

		w.Reset()
		err := o.encrypt(content, w)
		if err != nil {
			return nil, err
		}

		buf := w.Bytes()

		b1 := buf[bsize : bsize*2]
		b2 := buf[bsize*2 : bsize*3]

		if bytes.Equal(b1, b2) {
			out := buf[bsize*3:]

			// fmt.Println("aligning buf:")
			// hexFormatCopy(os.Stdout, bytes.NewReader(buf), 16)

			// fmt.Println("aligned ctext:")
			// hexFormatCopy(os.Stdout, bytes.NewReader(out), 16)

			return out, nil
		}
	}

	return nil, fmt.Errorf("cannot align buffer")
}

func recoverSecret(o *oracle, bsize int) (decoded []byte, err error) {
	prefixChars := make([]byte, bsize)
	// prefixSize := bsize - 1

	ctextW := bytes.NewBuffer(make([]byte, bsize*10))
	// ctextGuessW := bytes.NewBuffer(make([]byte, bsize*10))

	var block1 = make([]byte, bsize)
	// var block2 = make([]byte, bsize)

	for nthbyte := 0; nthbyte < bsize*10; nthbyte++ {
		var ctext []byte

		prefixSize := bsize - 1 - nthbyte%bsize
		prefix := prefixChars[0:prefixSize]

		ctextW.Reset()
		ctext, err = alignEncrypt(ctextW, o, prefix, bsize)
		// err = o.encrypt(bytes.NewReader(prefix), ctextW)
		if err != nil {
			return
		}

		nthblockStart := (nthbyte / bsize) * bsize

		copy(block1, ctext[nthblockStart:])

		guess := make([]byte, prefixSize+len(decoded)+1)
		copy(guess, prefix)
		copy(guess[prefixSize:], decoded)

		matchedByte := -1
		for i := 0; i < 128; i++ {
			guess[len(guess)-1] = byte(i)
			// fmt.Println("guess", nthbyte, len(guess), string(guess[prefixSize:]))

			ctextW.Reset()
			ctext, err = alignEncrypt(ctextW, o, guess, bsize)
			if err != nil {
				return
			}

			// copy(block2, ctext[nthblockStart:])
			block2 := ctext[nthblockStart : nthblockStart+bsize]

			// fmt.Println("block1 = block2", hex.EncodeToString(block1), hex.EncodeToString(block2))

			if bytes.Equal(block1, block2) {
				matchedByte = i
				break
			}
		}

		// end padding
		if matchedByte == 4 {
			return
		}

		if matchedByte >= 0 {
			// prefixSize -= 1
			decoded = append(decoded, byte(matchedByte))
		} else {
			return decoded, fmt.Errorf("cannot match byte at position: %d", nthbyte)
		}

	}

	return decoded, nil
}

func test() (err error) {
	var key [16]byte
	_, err = rand.Read(key[:])
	if err != nil {
		return err
	}

	secret, err := readbase64File("../12.txt")
	if err != nil {
		return
	}

	oracle, err := newOracle(key[:], secret)

	// prefix := []byte("hello ecb")

	// var ow bytes.Buffer
	// err = oracle.encrypt(bytes.NewReader(prefix), &ow)
	// if err != nil {
	// 	return
	// }
	// // ctext := ow.Bytes()

	// fmt.Println("ctext:")
	// hexFormatCopy(os.Stdout, &ow, 32)

	// for i := 0; i < 100; i++ {
	// 	recoverSecret(oracle, bsize)
	// }

	decoded, err := recoverSecret(oracle, bsize)
	if err != nil {
		return
	}

	fmt.Println("decoded:", string(decoded))

	// hexFormatCopy(os.Stdout, bytes.NewReader(decoded), 16)

	// fmt.Println("ctext", hex.EncodeToString(ctext))

	return nil
}

func main() {
	err := test()
	if err != nil {
		log.Fatalln(err)
	}

}
