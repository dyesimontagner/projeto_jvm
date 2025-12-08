public class SimpleLoop {
    public static int fatorial(int n) {
        int result = 1;
        int i = 2;
        while (i <= n) {
            result = result * i;
            i = i + 1;
        }
        return result;
    }

    public static void main(String[] args) {
        int valor = fatorial(5);  // Should be 120 (5!)
        // No output to avoid System.out.println
    }
}
