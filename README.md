# Colorize

Esse projeto tem como objetivo comparar a execução de um algorimo sequencial e uma respectiva versão utilizando a API OpenMP.


### Programa

O programa Colorize tem como objetivo colorir uma imagem originalmente em tons de cinza de acordo com os pixels de uma outra imagem fornecida. Para isso, ele realiza a comparação dos pixels mais semelhantes e atribui a cor de um pixel da imagem colorida para a imagem resultante.

O algoritmo utilizado é uma versão simplificada retirada do artigo Transferring Color to Greyscale Images dos autores: Tomihisa Welsh, Michael Ashikhmin e Klaus Mueller. Um pdf com o artigo original está na raiz desse repositório.

Obs.: Para padronização de código, assume-se que todas as imagens utilizadas devem ser PNG.

### Depedências

- GCC
- OpenMP
- Make
- Biblioteca stb_image

### Comandos de Instalação

Para compilar o programa utilizando a versão concorrente, utilize:
`make`

Para compilar o programa utilizando a versão sequencial, utilize:
`make seq`

Para remover o programa compilado, utilize:
`make clean`


### Comandos de Execução

Para executar o programa, utilize o comando abaixo, onde grayscale.png é o caminho da imagem que deve ser colorida, colorful.png é a imagem colorida que será usada como origem das cores e output.png é o nome da imagem resultante.

`./colorize grayscale.png colorful.png output.png`