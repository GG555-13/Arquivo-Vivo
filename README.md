# Projeto IDJ+

- Aluno - Matrícula: 
- Guilherme de Oliveira Mendes - 180113496
- Arthur Marmo Cathalá - 211043610

Esse trabalho foi realizado como parte da disciplina de Introdução ao Desenvolvimento de Jogos, e consiste em uma engine para um jogo 2d point and click feito em c++.

## Como Compilar e Executar

O projeto foi desenvolvido e testado em ambiente Linux e utiliza um `Makefile` para automatizar a compilação.

### Windows

No Windows, use MinGW-w64 com `g++` e `mingw32-make`.

O `Makefile` procura as bibliotecas SDL em `C:/SDL-2.32.10` por padrao. Se o SDL estiver em outro local, informe o caminho pela variavel `SDL_PATHS`.

Para compilar na raiz do projeto:

```bash
mingw32-make
```

Ou, se o SDL estiver em outro diretorio:

```bash
mingw32-make SDL_PATHS=C:/caminho/para/SDL
```

Para executar, mantenha `SDL2.dll`, `SDL2_image.dll`, `SDL2_mixer.dll` e `SDL2_ttf.dll` ao lado de `jogo.exe` ou deixe a pasta `C:/SDL-2.32.10/bin` no `PATH`.

### Linux
### 1. Requisitos

Antes de compilar, certifique-se de que você tem as bibliotecas de desenvolvimento do SDL2 instaladas. Em sistemas baseados em Debian/Ubuntu, você pode instalá-las com o seguinte comando:

```bash
sudo apt-get update
sudo apt-get install build-essential libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev
```

### 2. Compilação

Com os requisitos instalados, basta executar o comando `make` na pasta raiz do projeto:

```bash
make
```
Isso irá compilar todos os arquivos-fonte e criar um executável chamado `jogo` na raiz do projeto.

### 3. Execução

Para rodar o jogo, execute o arquivo gerado:

```bash
./jogo
```

### Limpando os Arquivos de Build

Para remover o executável e os arquivos de compilação intermediários (da pasta `obj/`), use o comando:
```bash
make clean
```
