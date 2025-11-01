# cfg2yacc Documentation Index

## 📖 Documentation Files

### For Getting Started
1. **[QUICK_START.md](QUICK_START.md)** - Get started in 5 minutes
2. **[README.md](README.md)** - Project overview and quick reference

### For Detailed Usage
3. **[USAGE_GUIDE.md](USAGE_GUIDE.md)** - Complete usage guide with:
   - What is cfg2yacc?
   - How to write EBNF grammars
   - All command-line options
   - Grammar syntax reference
   - Complete examples
   - Troubleshooting

### For Quick Reference
4. **[EXAMPLES.md](EXAMPLES.md)** - Quick copy-paste examples

### Technical Documentation
5. **[REPORT.md](REPORT.md)** - Technical report and algorithms

## 🚀 Quick Links

- **New User?** → Start with [QUICK_START.md](QUICK_START.md)
- **Writing Grammars?** → See [USAGE_GUIDE.md](USAGE_GUIDE.md) "Writing EBNF Grammars"
- **Need Examples?** → Check [EXAMPLES.md](EXAMPLES.md)
- **Command Reference?** → See [USAGE_GUIDE.md](USAGE_GUIDE.md) "Common Run Commands"

## 📝 Typical Workflow

1. **Learn the basics**: Read [QUICK_START.md](QUICK_START.md)
2. **Understand grammar syntax**: See [USAGE_GUIDE.md](USAGE_GUIDE.md) sections on grammar format
3. **Try examples**: Follow examples in [EXAMPLES.md](EXAMPLES.md)
4. **Reference details**: Use [USAGE_GUIDE.md](USAGE_GUIDE.md) as needed

## 🎯 Most Common Tasks

### Convert a grammar
```bash
./cfg2yacc -i mygrammar.ebnf -o mygrammar.y
```
See: [QUICK_START.md](QUICK_START.md) Step 3

### Generate with lexer
```bash
./cfg2yacc -i mygrammar.ebnf -o mygrammar.y --emit-lex mygrammar.l
```
See: [USAGE_GUIDE.md](USAGE_GUIDE.md) "With Lexer Generation"

### Understand grammar syntax
See: [USAGE_GUIDE.md](USAGE_GUIDE.md) "Writing EBNF Grammars"

### Troubleshoot errors
See: [USAGE_GUIDE.md](USAGE_GUIDE.md) "Troubleshooting"

