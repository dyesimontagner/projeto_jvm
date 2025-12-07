// Arquivo: LoopTest.java
public class LoopTest {
    public static void main(String[] args) {
        int sum = 0;
        for (int i = 1; i <= 5; i++) {
            sum += i;
            if (i % 2 == 0) {
                System.out.println("i é par: " + i);
            }
        }
        System.out.println("Soma total: " + sum); // Esperado: 15
    }
}