package main

import (
	//"fmt"
	"testing"
	//"time"
)

func BenchmarkAdd(b *testing.B) {
	//fmt.Printf("%d\n", b.N)
	var i int = 1
	//var sum int = 0
	
	for ; i <= b.N; i++ {
		Sum()
	}
}
