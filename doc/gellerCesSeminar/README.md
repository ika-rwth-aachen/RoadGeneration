# LaTeX Vorlage ika

Inoffizielle Vorlage für Abschlussarbeiten am ika bzw. b6

# Vorgehen

## Titelblatt

Word öffnen, Titelblattvorlage öffnen, Felder ausfüllen, als `CoverSheet.pdf` im Hauptordner speichern.
Alternativ auch ein anderer Name, dann halt in `main.tex` anpassen.

## Glossaries

Für das Abkürzungsverzeichnis und das Symbolverzeichnis muss `makeindex` verwendet werden.
Der Befehl für die Abkürzungen lautet:
 
```bash
makeindex main.acn -s main.ist -t main.alg -o main.acr
```

Für die Symbole kann auch `makeindex` verwendet werden.
Hier lautet der Befehl

```bash
makeindex main.syg -s main.ist -t main.slg -o main.syi
```

## Literaturverzeichnis

Das Literaturverzeichnis wird mit `biber` gebaut.
Der Befehl dafür lautet dann

```bash
biber main
```

# TODO

Mehr Beschreibungen!
