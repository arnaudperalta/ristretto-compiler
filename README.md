# ristretto-compiler
Simplist C Language compiler for JVM

## Manual

Compile a .ris file into .class file

`$ ./ristretto facto.ris`

Execute the obtained .class file with the JVM

`$ java -noverify Facto`

Observe the content of the obtained .class file

`$ javap -c -s -v -p -l -constants Exemple.class`


## Ristretto syntax

### Main function
```c
void main(void) {
     [...]
}
```

### Output instructions
```c
void main(void) {
    print "Ce message s'affiche ";
    println "sur la même ligne";
    print "Je viens de sauter une ligne";
}
```

Output :
```
Ce message s'affiche sur la même ligne
Je viens de sauter une ligne
```

### Global variables
```c
int var1 = 0;
float var2 = 0.0;
bool var3 = true;

void main(void) {
    println var1;
}
```

### Functions
```c
int facto(int n) {
    if (n == 0) {
	    return 1;
    }
    return facto(n - 1) * n;
}
```

### Conditions
```c
void main(void) {
    if (true) {
        println "Je suis affiché";
    } else {
        println "Je ne suis pas affiché";
    }
}
```

### Loops
```c
int i = 0;
while (i < 5) {
    println i;
    i = i + 1;
}
```

## Some examples
### Factorial
```c
int facto(int n) {
    if (n == 0) {
	    return 1;
    }
    return facto(n - 1) * n;
}
```

### Gregory-Leibniz Series
```c
void gregory_leibniz(int n) {
    float pi = 0.0;
    float denominateur = 1.0;
    boolean sign = true;
    while (n > 0) {
        if (sign) {
            pi = pi + 1.0 / denominateur;
        } else {
            pi = pi - 1.0 / denominateur;
        }
        sign = !sign;
        println 4.0 * pi;
        denominateur = denominateur + 2.0;
        n = n - 1;
    }
}
```
