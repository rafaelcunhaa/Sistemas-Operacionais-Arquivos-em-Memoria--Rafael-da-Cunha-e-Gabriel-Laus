# Sistemas-Operacionais-Arquivos-em-Memoria--Rafael-da-Cunha-e-Gabriel-Laus

Trabalho avaliativo da disciplina de Sistemas Operacionais – UNIVALI  
Professor: Michael D C Alves  
Curso: Ciência da Computação – 5º Período  
---

## 🛠️ Como compilar e executar

### Compilação

Copie o codigo que está no arquivo arq_em_memoria.c em um compilador compativel com a linguagem C ou faça um git clone desse proprio git.
Logo em seguida execute o programa, rodando você pode testar o programa usando esses comando um de cada vez.

mkdir docs

cd docs

touch readme.txt

echo readme.txt OPAAAAAAAAAA

cat readme.txt

chmod 0 readme.txt

cat readme.txt  # Permissão negada

### Comandos disponíveis

- mkdir nome – Cria um novo diretório
- cd nome – Acessa um subdiretório
- cd .. – Volta para o diretório pai
- ls – Lista arquivos e diretórios
- touch nome – Cria um arquivo vazio
- echo nome conteúdo – Escreve conteúdo no arquivo
- cat nome – Lê o conteúdo do arquivo
- rm nome – Remove o arquivo
- chmod perm nome – Altera as permissões RWX (ex: chmod 7 arquivo)
- exit – Encerra o simulador

## Estruturas e escolhas de design

### Estrutura de Diretórios (Árvore)

Utilizamos a estrutura `struct Diretorio`, com ponteiros para subdiretórios e arquivos, além de um ponteiro para o diretório pai. Isso permite:

* Navegação com `cd`, `cd ..`
* Agrupamento lógico de arquivos
* Simulação eficiente da hierarquia do sistema de arquivos

### Arquivos e File Control Block (FCB)

Cada arquivo é representado por um `struct Arquivo`, que contém:

* Nome, tipo, tamanho
* Timestamps (`criado`, `modificado`, `acessado`)
* ID único (simulando um inode)
* Permissões RWX (bitfields)
* Conteúdo textual (string)
* Blocos de dados simulados (índices de alocação)

### Ponteiros e Structs

* Diretórios armazenam ponteiros para structs de arquivos e subdiretórios.
* O uso de ponteiros permite que os arquivos e diretórios existam independentemente em memória e sejam facilmente manipuláveis.

---

## Conceitos Teóricos Aplicados

### Conceito de Arquivo e Atributos

* Atributos implementados: nome, tamanho, tipo, timestamps, permissões e ID.
* Cada arquivo tem um inode simulado com ID único e bloco de dados associados.

### Operações com Arquivos

Implementadas:

* `touch` → cria arquivo
* `echo` → escreve conteúdo
* `cat` → lê conteúdo
* `rm` → remove arquivo
* `chmod` → altera permissões

### Representação do FCB e Inode

* Cada arquivo tem seu próprio FCB (`struct Arquivo`) armazenando metadados e blocos.
* O campo `id` simula o inode do sistema de arquivos real.

### Estrutura em Árvore

* Diretórios organizados em árvore permitem:

  * Nomeação clara e sem conflitos
  * Agrupamento lógico
  * Eficiência em buscas e navegação

### Controle de Acesso (chmod + RWX)

* Permissões implementadas usando bitmask:

  * `4 = leitura`, `2 = escrita`, `1 = execução`
  * Comando `chmod` aceita valores como `7` (RWX), `6` (RW-), `4` (R--)
* Verificação de permissões ao executar `cat`, `echo`, etc.

### Simulação de Alocação de Blocos

* Um vetor `char disco[]` representa o disco lógico.
* Cada arquivo aloca blocos usando índices (`int blocos[]`).
* Tipo de alocação: **contínua**, mas simulável como encadeada/indexada.


---

## 👨‍💻 Autor

Rafael Cunha e Gabriel Laus

UNIVALI – Sistemas Operacionais











