// Arquivo: MemoryEater.java
import java.util.ArrayList;
import java.util.List;

public class MemoryEater {
    public static void main(String[] args) {
        System.out.println("Tentando consumir memória...");
        // Cria uma lista para manter referências a objetos grandes
        List<byte[]> list = new ArrayList<>();
        
        try {
            while (true) {
                // Aloca um grande array de bytes (1 Megabyte) a cada iteração
                byte[] data = new byte[1024 * 1024]; 
                list.add(data);
            }
        } catch (OutOfMemoryError e) {
            System.out.println("Erro de Memória! O Garbage Collector não conseguiu liberar espaço suficiente.");
            System.out.println("Total de Megabytes alocados antes da falha: " + list.size());
        }
    }
}