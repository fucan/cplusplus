// test project main.go
package main

import (
	"errors"
	"fmt"
	"log"
	"os"
	"os/exec"
	"strings"
	"time"
)

func Sum() {
	//start := time.Now()
	var i int = 0
	var sum int = 0
	for ; i < 10000; i++ {
		//time.Sleep(1*time.Microsecond)
		sum += i
	}
	//end := time.Now()

	//fmt.Printf("%d ns\n", end.Sub(start))
}
func add(ch chan int) {
	var i int = 0
	for i < 10 {
		i++
		fmt.Println(i)
	}
	ch <- 1
}

func Array() {
	arr := [5]int{1, 2, 3, 4, 5}
	for i, v := range arr {
		fmt.Println("第", i+1, "个数是", v)
	}
}

func Slice() {
	var arr [5]string = [5]string{"1", "e", "3", "4", "5"}
	var sli []string = arr[0:1]
	sli = append(sli, "6", "7")
	for i, v := range sli {
		fmt.Println("第", i+1, "个数是", v)
	}
}

type integer int

func (a integer) Less(b integer) bool {
	return a < b
}

func MulRet() (a int, err error) {
	errors.New("error")
	return 1, nil
}

func somefunc() {
	for i := 0; i < 10; i++ {
		hFile, err := os.Open("1.txt")
		if err != nil {
			return
		}
		hFile.Close()
		defer func(i int) {
			fmt.Println("Close defer", i) //获取的是

		}(i) //放在匿名函数里
	}
}

type File struct {
	//	...
}

func (f *File) Read(buf []byte) (n int, err error) {
	for i := 0; i < len(buf); i++ {
		buf[i] = byte(i + 1)
	}
	return 0, nil
}
func (f *File) Write(buf []byte) (n int, err error) {
	return 0, nil
}
func (f *File) Seek(off int64, whence int) (pos int64, err error) {
	return 0, nil
}
func (f *File) Close() (err error) {
	src, err := os.Open("1.txt")
	if err != nil {
		fmt.Println(err.Error())
		return
	}

	defer fmt.Println("open") //最后运行
	fmt.Println(src.Name())
	defer func() {
		fmt.Println("Close defer")
		src.Close()
	}()
	//panic("something error")
	fmt.Println("Close normal")
	src.Close()
	return err
}

type IFile interface {
	Read(buf []byte) (n int, err error)
	Write(buf []byte) (n int, err error)
	Seek(off int64, whence int) (pos int64, err error)
	Close() error
}

type IReader interface {
	Read(buf []byte) (n int, err error)
}

type Iwriter interface {
	Write(buf []byte) (n int, err error)
}

type ICloser interface {
	Close() error
}

const (
	_        = iota             // iota = 0
	KB int64 = 1 << (10 * iota) // iota = 1
	MB                          // 与 KB 表达式相同，但 iota = 2
	GB
	TB
)

func test(x [2]int) {
	fmt.Printf("x: %p\n", &x)
	x[1] = 1000
}

func test1(x []int) {
	fmt.Printf("x: %p\n", &x)
	x[1] = 1000
}

func main() {
	Sum()
	fmt.Println(TB)
	/*var str string = "222"
	//str := "1111"
	fmt.Println(str)
	var i int = 1
	i += 2
	j := i + 3
	fmt.Printf("%d\n", i)
	fmt.Print(j)
	//ch := make(chan int)
	//go add(ch)
	value := 1 //<-ch
	fmt.Println("Hello World!", value)
	Slice()

	var a integer = 1
	if a.Less(2) {
		fmt.Println(a, "Less 2")
	}
	fmt.Println(MulRet())*/
	/*somefunc()
	var file1 IFile = new(File)
	var buf []byte = make([]byte, 10)
	file1.Write(buf)
	var file2 IReader = new(File)
	file2.Read(buf)
	//fmt.Println(buf)
	var file3 Iwriter = new(File)
	file3.Write(buf)
	var file4 ICloser = new(File)
	defer file4.Close()*/

	/*a := [2]int{}
	fmt.Printf("a: %p\n", &a)
	test(a)
	fmt.Println(&a[0])
	b := []int{0, 0}
	fmt.Printf("a: %p\n", &b)
	test1(b)
	fmt.Println(&b[0])*/
	data := []int{0, 1, 2, 3, 4, 5, 6, 7, 8, 7}
	s := data[1:3:4]
	fmt.Println(&s[0], &data[0])
	fmt.Println(s, data)
	fmt.Println(cap(s))
	s = append(s, 100)
	fmt.Println(&s[0], &data[0])
	fmt.Println(s, data)
	fmt.Println(os.Getppid())
	fmt.Println(os.Getpid())
	exec.Command("aaa")
	var namemap = make(map[string]string, 10)
	namemap["fucan"] = "haha"
	//namemap
	fmt.Println(namemap)

	var cl chan string = make(chan string)
	go func() {
		cl <- "no"
		fmt.Println("1")
		cl <- "ok"
		fmt.Println("2")
		time.Sleep(2 * time.Second)
		cl <- "no ok"
	}()

	var i = 0
	{
		i := 1
		fmt.Println(i)
	}
	fmt.Println(i)
	var slice1 []byte = []byte{'f', 'u', 'c', 'a', 'n'}
	slice2 := slice1[1:]
	slice2[0] = 'A'
	fmt.Printf("%p\n%p\n", &slice1[1], &slice2[0])

	slice2 = append(slice1, 'h')
	fmt.Printf("%p\n%p\n", &slice1[1], &slice2[0])
	var str1 = "ABC"
	var str2 = str1[0:1]
	str3 := strings.Replace(str1, "A", "B", 1)
	var slice3 = []byte(str1)
	slice3[0] = 'B'
	index := strings.Index(str1, "C")
	log.Printf("%d", index)
	fmt.Printf("%s\n%s\n%s\n", str1, str2, str3)

	var c = make(chan chan bool)
	go func() {
		for {
			fmt.Println(<-c)
		}
	}()
	/*for {
		c <- make(chan bool)
	}*/
	var anymap = make(map[interface{}]interface{})
	anymap[true] = false
	anymap["2"] = "2"

	for key, _ := range anymap {

		switch keytype := key.(type) {
		case bool:
			fmt.Println("bool")
		default:
			fmt.Println("key:", keytype)
		}

		//fmt.Println(value.(type))
	}
	//n := 15
	//var arr []int
}
