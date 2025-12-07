// Arquivo: Counter.java
public class Counter {

    public static void main(String[] args) {
        int result = 0;
        int limit = 5;

        // Loop de 1 a 5
        for (int i = 1; i <= limit; i++) {
            result = result + i; // iadd, istore, iload
        }

        // Simulação de System.out.println(15)
        System.out.println("Resultado: " + result); 
        // Valor Esperado: 15
    }
}