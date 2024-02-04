# Phosphorus - A Physics Engine for Movement Analysis

We provide a easy language to describe some fields, just like this:

```
# To start a comment
field:
E=xxx #(F1=Eq)
g=xxx #(F2=mg)
deg=xxx #(0-360)

mag_field:
B=xxx #(F=qvB)
deg=xxx #(in/out)

object:
m=xxx
q=xxx
posX=xxx
posY=xxx
v0=xxx
v0Deg=xxx

repeat: # Start analysis
div=xxx s # s/ms/us

```

We use toolchain as follows: Lex(GNU FLex 2.6.4), Yacc(GNU Bison 3.8.2), Matplotlib, GNU C Compiler(13.2.1, must 11 above), Python(3.11.6, must 3 above), CMake(3.28.2, must 3.16.3 above), ninja(1.11.1)

Let's introduce the methods:

1. Force mixing
F1=Fx1+Fy1;
F2=Fx2+Fy2;
F1+F2=F
2. 


The Program use SI, so please check your parameters before you use this program.