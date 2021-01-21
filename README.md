# lang-processing
[![CI](https://github.com/Hiroya-W/lang-processing/workflows/CI/badge.svg)](https://github.com/Hiroya-W/lang-processing/actions?query=workflow%3ACI)
![](https://img.shields.io/badge/license-MIT-blue.svg?style=flat)

言語処理プログラミングの講義において、C言語を用いてプログラミング言語MPPLのコンパイラを作成するために用いたリポジトリ。

演習は次の4ステップでプログラムを作成していく。

## 課題1:字句の出現頻度表の作成

字句(トークン)がそれぞれ何個出現したかを数え，出力するプログラムを作成する．

### Usage

```
$ cd program1
$ make token-list
$ ./token-list samples/sample11.mpl

      NAME:    14
        "Identifier" "data"     3
        "Identifier" "sum"      5
        "Identifier" "n"        5
        "Identifier" "sample11" 1
   program:     1
       var:     1
     begin:     2
       end:     2
     while:     1
        do:     1
   integer:     1
    readln:     2
   writeln:     2
    NUMBER:     3
    STRING:     2
         +:     1
         -:     1
         >:     1
         (:     4
         ):     4
        :=:     3
         .:     1
         ,:     3
         ::     1
         ;:     8
```

## 課題2:プリティプリンタの作成

構文エラーがなければ，入力されたプログラムをプリティプリントした結果を出力し，構文エラーがあれば，そのエラーの情報（エラーの箇所，内容等）を少なくとも一つ出力するプログラムを作成する．

### Usage

```
$ cd program2
$ make main
$ ./main samples/sample25.mpl

program ifst;
var ch : char;
begin
    readln(ch);
    if ch = 'a' then
        writeln('It is ''a'' ')
    else
        writeln('It is not ''a'' ')
end.⏎ 
```

## 課題3:クロスリファレンサの作成

コンパイルエラー，すなわち，構文エラーもしくは制約エラー（型の不一致や未定義な変数の出現等）がなければ，クロスリファレンス表を出力し，エラーがあれば，そのエラーの情報（エラーの箇所，内容等）を少なくとも一つ出力するプログラムを作成する．

### Usage

```
$ cd program3
$ make main
$ ./main samples/sample31p.mpl

-----------------------------------------------------------------------------------------
Name                Type                                         Def. | Ref.
a                   integer                                         2 | 20,22,22
p                   procedure(char)                                 3 | 21
a:p                 char                                            3 | 6
b                   char                                            8 | 20,21
q                   procedure(integer)                              9 | 22,22
b:q                 integer                                         9 | 15
a:q                 boolean                                        10 | 15,16
q:q                 integer                                        11 | 14,15
c                   integer                                        18 |
-----------------------------------------------------------------------------------------
```

## 課題4:コンパイラの作成

コンパイルエラー，すなわち，構文エラーもしくは制約エラー（型の不一致や未定義な変数の出現等）があれば，そのエラーの情報（エラーの箇所，内容等）を少なくとも一つ出力し，エラーがなければ，オブジェクトプログラムとして，CASL IIのプログラムを出力するプログラム（すなわちコンパイラ）を作成する．

### Usage

```
$ cd program4
$ make main
$ cp ../samples/program1/sample11.mpl ./
$ ./main sample11.mpl
$ vim sample11.csl
```
