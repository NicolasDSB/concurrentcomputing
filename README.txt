Necessário a header list_int.h na pasta e que o código (disponível no LAB 6) list_int.c seja compilado junto.

Eu implementei a mesma versão leitor/escritor dos slides da aula, porém adicionei uma variável querEscr.
Quando alguma thread entra para escrever ela incrementa querEscr e, se tiver leitores no momento, se bloqueia no while.
Isso faz com que não possam entrar novos leitores e o último leitor vai desbloquear o caminho para o escritor passar (pois não terão mais leitores).