# Resumo de Testes da JVM

## Testes Implementados

| Arquivo | Objetivo | Opcodes Necessários | Status |
|---------|----------|-------------------|--------|
| **Mini.java** | Aritmética básica (+ load/store) | iconst, iload, istore, iadd, isub, imul |  RODA |
| **TestDiv.java** | Divisão inteira | iload, idiv, istore |  RODA |
| **TestIfEq.java** | Condicional if_icmpeq/if_icmpne | if_icmpeq, if_icmpne |  RODA |
| **TestLdc.java** | Carregamento de constante (string) | ldc, astore |  RODA |
| **TestNew.java** | Criação de objeto | new, dup, astore, invokespecial |  RODA |
| **SimpleLoop.java** | Loop com controle de fluxo | iload, istore, iconst, imul, iadd, goto, if_icmpgt, ireturn | ⏳ PARCIAL (invokestatic não resolve) |
| **ReturnTest.java** | Chamada de método que retorna valor | bipush, invokestatic, iload, iadd, istore, ireturn | ⏳ PARCIAL (invokestatic placeholder) |
| **Fibonacci.java** | Recursão (if_icmple + invokestatic) | iload, if_icmple, isub, invokestatic, iadd, ireturn | ⏳ PARCIAL (invokestatic não resolve) |
| **Teste.java** | Usa System.out.println (getstatic) | getstatic, invokespecial | ❌ FALHA (getstatic não implementado) |

## Opcodes Implementados

###  Completamente Implementados:
- **Constantes**: `iconst_m1`, `iconst_0` a `iconst_5`, `bipush`
- **Carregamento**: `iload`, `iload_0` a `iload_3`, `aload`, `aload_0`
- **Armazenamento**: `istore`, `istore_0` a `istore_3`, `astore`, `astore_0` a `astore_3`
- **Aritmética**: `iadd`, `isub`, `imul`, `idiv`
- **Controle**: `goto`, `ifeq`, `ifne`, `ifle`, `if_icmpeq`, `if_icmpne`, `if_icmpgt`, `if_icmple`, `if_icmpge`
- **Stack**: `dup` (duplica topo)
- **Referências**: `ldc`, `ldc_w` (load constant)
- **Objetos**: `new` (criação de objeto)
- **Campos**: `getfield`, `putfield` (simplificado)
- **Métodos**: `invokespecial` (placeholder), `return`, `ireturn`
- **Meta**: `nop` (no-op)

### Parcialmente Implementados:
- `invokestatic` - Lê operandos mas não chama método (requer resolução de método)

### Não Implementados:
- `getstatic` (acesso a campos estáticos - requer System class integration)
- `invokevirtual` (chamadas virtuais)
- Outros opcodes não utilizados pelos testes

## Como Testar

```bash
# Testes que funcionam 100%:
bin\leitor -jvm test_files\Mini.class main
bin\leitor -jvm test_files\TestDiv.class main
bin\leitor -jvm test_files\TestIfEq.class main
bin\leitor -jvm test_files\TestLdc.class main
bin\leitor -jvm test_files\TestNew.class main

# Testes parciais (invokestatic não resolve método):
bin\leitor -jvm test_files\ReturnTest.class main
bin\leitor -jvm test_files\SimpleLoop.class main
bin\leitor -jvm test_files\Fibonacci.class main

# Teste que falha (requer getstatic):
bin\leitor -jvm test_files\Teste.class main
```

## Compilação

```bash
make
```

A compilação usa GCC c99 com todas as sources em `src/` - agora inclui:
- `opcodes_const_load_store.c` - Constantes, load/store, stack
- `opcodes_aritmetica.c` - Operações aritméticas
- `opcodes_controle.c` - Branching e jumps
- `opcodes_metodos.c` - Return statements
- `opcodes_referencias.c` - Constant pool loading (ldc)
- `opcodes_objetos.c` - Object allocation e field access

## Próximas Prioridades

1. **Implementar `idiv`** (divisão) - opcao simples
2. **Implementar `ifeq`** e `if_icmpeq` - comparações básicas
3. **Completar `invokestatic`** - resolver método e criar novo frame
4. **Implementar `ldc`** - carregar constante do constant pool
5. **Implementar `new`** e heap - alocação de objetos
