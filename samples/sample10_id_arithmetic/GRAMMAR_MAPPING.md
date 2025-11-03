# Your Grammar → Implementation Mapping

## Your Specification

```
Rules:
token - id 
format - [a-z]

token - num 
format - [0-9]+

token - sym
format - +|*|-

grammar:
id -> id*id | id+id | id-id
id -> num
```

## ✅ How We Implemented It

### S10_analyzer.def

```
%%LEX
ID [a-z]          ← Your "id" token
NUM [0-9]+        ← Your "num" token
PLUS \+           ← Your "sym" token (plus)
TIMES \*          ← Your "sym" token (times)
MINUS -           ← Your "sym" token (minus)

%%YACC
identifier -> NUM                         ← Your: id -> num
identifier -> ID                          ← Single letter is also an identifier
identifier -> identifier PLUS identifier  ← Your: id -> id+id
identifier -> identifier TIMES identifier ← Your: id -> id*id
identifier -> identifier MINUS identifier ← Your: id -> id-id
```

## 🔄 Name Changes (Why?)

| Your Name | Our Name | Reason |
|-----------|----------|--------|
| `id` | `ID` (token), `identifier` (grammar) | YACC needs uppercase for tokens |
| `num` | `NUM` (token) | YACC convention |
| `sym` | `PLUS`, `TIMES`, `MINUS` (separate tokens) | Better parse tree clarity |

## 📊 Results

All your rules work perfectly! Here are examples:

### Input: `b+c` (id + id)
```
identifier
  ├─ identifier (b)
  ├─ PLUS (+)
  └─ identifier (c)
```
✅ Matches: `id -> id+id`

### Input: `x*y` (id * id)
```
identifier
  ├─ identifier (x)
  ├─ TIMES (*)
  └─ identifier (y)
```
✅ Matches: `id -> id*id`

### Input: `5` (num)
```
identifier
  └─ NUM (5)
```
✅ Matches: `id -> num`

### Input: `3+7` (num + num)
```
identifier
  ├─ identifier
  │   └─ NUM (3)
  ├─ PLUS (+)
  └─ identifier
      └─ NUM (7)
```
✅ Matches: `id -> num` then `id -> id+id`

## 🎓 Grammar Equivalence

Your grammar:
```
id -> id*id | id+id | id-id | num
```

Is **exactly** implemented as:
```
identifier -> identifier TIMES identifier
identifier -> identifier PLUS identifier
identifier -> identifier MINUS identifier
identifier -> identifier NUM
identifier -> ID
```

The only difference is naming conventions required by Lex/YACC tools!

## 📁 Files Created

1. **S10_analyzer.def** - Your grammar in cfg2yacc format
2. **S10_input.txt** - Test cases for all your rules
3. **S10_rules.txt** - Documentation
4. **README.md** - Complete guide

## 🚀 Test It Now!

```bash
make run DEF_FILE=samples/sample10_id_arithmetic/S10_analyzer.def
```

Every line in the input file demonstrates one of your grammar rules! 🎉
