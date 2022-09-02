# ASCII art
Author: sentisso

# Dependencies
- `g++` - this compiler _should_ already be installed on your linux system
- `make` - this _should_ also already be installed on your linux system
- `libjpeg` - install package `libjpeg-dev` (for example `yum -y install libjpeg*` on RHEL 8)
- `libpng` - install package `libpng-dev` (for example `yum -y install libpng*` on RHEL 8)

# How to run
1. download or clone the project
2. `cd ascii-art/`
3. `make run`
4. follow the instructions on the screen
5. you can try some of the images in the assets (eg. `./assets/tests/ok/jpg/a21.jpg` <- toast cat)

# Usage
- **zooming:** the program reacts to the terminal text zoom (so if you zoom out the terminal, the ASCII image will be more detailed)
- **resizing:** also you can resize the terminal window to make the picture bigger
- **coloring:** do not forget to enable colors by pressing "c"! If your terminal supports it...

# User documentation in czech
## Chování programu:
- **spuštění:** Po spuštění bude uživatel vyzván k definování ASCII přechodu, pomocí kterého by rád generoval obrázky
- **náhrání prvního obrázku:**
    1. Nahrávání bude vyřešeno pomocí textového pole, kam uživatel zadá cestu k danému obrázku (např. absolutně`/home/user/Desktop/sample.jpg` nebo relativně `./sample.jpg`) (viz rozhraní #1)
    2. Po nahrání bude obrázek automaticky převeden do ASCII podoby a zobrazen na hlavní ploše (viz rozhraní #2)
- **nahrávání dalších obrázků a pohyb mezi nimi:**
    1. Uživateli je umožněno nahrát další obrázek pomocí klávesy "o" (jako open) (viz rozhraní #3)
    2. po úspěšném nahrání bude obrázek automaticky převeden do ASCII podoby, zobrazen na hlavní ploše a jeho miniatura umístěna na konec fronty na spodní části obrazovky (viz rozhraní #2)
    3. je-li v programu nahráno více obrázků, tak se uživatel může mezi nimi "pohybovat" pomocí levé a pravé šipky a tím si každý zobrazit na hlavní ploše nebo spustit animaci přehrávání pomocí klávesy "s" (automatické přepínání obrázků).
- **editace obrázků:**
    1. po stisknutí klávesy "e" (jako edit) bude uživateli zpřístupněno pár nastavení, které budou umožňovat vizuální editaci celého obrázku (viz rozhraní #6)
    2. intuitivním (doufejme) pohybem může v nastavení změnit několik atributů a následně změny uložit nebo zrušit bez uložení
- **mazání a ukládání obrázků:**
    - když uživatel stiskne klávesu "d" (jako delete), tak bude otázán, zda chce smazat aktuálně zobrazený obrázek (viz rozhraní #3) (pozn. "fyzický" obrázek z filesystému smazán nebude, pouze jeho ASCII podoba v paměti programu)
- **úprava zobrazení obrázků:**
    - program podporuje vypsání ASCII-art také i barevně (tzn. vypsání barevných znaků). Zapínání/vypínaní barevnosti je možné pomocí klávesy "c" (jako color).
      - ovšem pouze pokud uživatelův terminál podporuje 256 Xterm barev
    - pokud terminál podporuje zvětšování a zmenšivání textu (např. pomocí CTRL+), tak toto program umí zachytit tím je možné např. zvětšovat rozlišení obrázků
    - v případě zapnutých barev je umožněno měnit barvu i pozadí obrázku pomocí klávesy "b" (jako background)
      - barvy jsou: černá, bílá a původní pozadí terminálu 
    - je také umožněno schovat miniatury na spodu obrazovky pomocí klávesy "t", za důvodem více místa na obrazovce

### Ovládání:
- otevření nového obrázku: o
- otevření editačního menu: e
- zobrazení předešlého obrázku: ◄ (šipka vlevo)
- zobrazení dalšího obrázku: ► (šipka vpravo)
- přesunutí obrázku ve frontě doleva: j
- přesunutí obrázku ve frontě doprava: l
- smazání právě zobrazeného obrázku: d
- zobrazení/schovaání miniatur obrázků: t
- zapnutí/vypnutí barev: c
- změna pozadí obrázku: b
- ukončení programu: q


#### `cloc src/` output
```
      31 text files.
      31 unique files.
       0 files ignored.

github.com/AlDanial/cloc v 1.92  T=0.01 s (2536.4 files/s, 314022.6 lines/s)
-------------------------------------------------------------------------------
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
C++                             15            560            276           1726
C/C++ Header                    16            228            612            436
-------------------------------------------------------------------------------
SUM:                            31            788            888           2162
-------------------------------------------------------------------------------
```
