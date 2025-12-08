public class Mini {
    public static void main(String[] args) {
        int a = 2;       // iconst_2, istore_1
        int b = 3;       // iconst_3, istore_2
        int c = a + b;   // iload_1, iload_2, iadd, istore_3
        int d = c + 4;   // iconst_4, iload_3, iadd, istore 4
        int e = d - 1;   // iconst_1, iload 4, isub, istore 5
        // no output; just exercise arithmetic/loads/stores
    }
}
