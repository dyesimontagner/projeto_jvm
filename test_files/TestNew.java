// Teste de new (criar novo objeto)
public class TestNew {
    int count = 0;
    
    public TestNew() {
        count = 0;
    }
    
    public static void main(String[] args) {
        TestNew c = new TestNew();  // Usa new para criar objeto
        // c.count = 5;  // Usaria putfield
    }
}
