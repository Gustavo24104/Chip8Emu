## Sobre 

escreva aqui gggg!!

## Pré-requisitos

Além das bibliotecas já inclusas em um ambiente C padrão, será necessário a raylib, que pode ser instalado no Windows, Linux, MacOS, FreeBSD entre outros. Por fim, será necessário um arquivo .ch8 para testes.

### Instalação em sistemas Windows

Utilize o [instalador](https://raysan5.itch.io/raylib/download/eyJpZCI6ODUzMzEsImV4cGlyZXMiOjE3NjU0NTkxNDJ9.gvinH4olS58Q1hQFM5644Zbh1j8%3d)

### Instalação em sistemas Linux

#### Ubuntu e derivados
```[bash]
sudo add-apt-repository ppa:texus/raylib
sudo apt install libraylib5-dev
```

#### Fedora
```[bash]
sudo dnf install raylib
```

#### Arch Linux e derivados
```[bash]
sudo pacman -S raylib
```

### Instalação em sistemas FreeBSD
```[bash]
sudo pkg install raylib
```

### Instalação em sistemas MacOS

Siga esse [guia](https://github.com/raysan5/raylib/wiki/Working-on-macOS) para a instalação.

### Arquivo de testes

Para o programa funcionar, é necessário um arquivo .ch8, que pode ser fornecido pelo próprio usuário ou retirado deste [repositório](https://github.com/Timendus/chip8-test-suite) na pasta [bin](https://github.com/Timendus/chip8-test-suite/tree/main/bin). Escolha qualquer um dos arquivos .ch8, e o coloque no mesmo diretório dos arquivos fonte do projeto, depois troque o nome ou caminho do arquivo na main, e finalmente, execute o seguinte comando:
```[bash]
gcc -mavx2 cpu.c cpu.h main.c processing.c processing.h -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o teste
```

Será criado um executável chamado 'teste', e então, basta executá-lo !