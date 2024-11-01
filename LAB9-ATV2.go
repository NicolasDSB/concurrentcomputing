// introduz canais na aplicação Go
package main

import (
	"fmt"
	"time"
)

func tarefa(str chan string) {
	fmt.Println(<-str)
	//escreve no canal
	str <- "Oi Main, bom dia, tudo bem?"
	fmt.Println(<-str)
	str <- "Certo, entendido."
	time.Sleep(time.Millisecond * 2)
	fmt.Println("Terminando...")
}

func main() {
	//cria um canal de comunicacao nao-bufferizado
	str := make(chan string)
	go tarefa(str)
	str <- "Olá, goroutine, bom dia"
	fmt.Println(<-str)
	str <- "Tudo bem! Vou terminar tá?"
	fmt.Println(<-str)
	time.Sleep(time.Millisecond * 20)
	fmt.Println("Terminando...\n")
}
