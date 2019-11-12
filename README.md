# roadGeneration - Seminararbeit Christian Geller 

Entwicklung eines Tools zur automatisierten Erstellung komplexer Straßennetze ausgehend von einer logischen Beschreibung

## Struktur des Git Repositories

* bin: enthält die Executable und einige Beispiel Input Files
* doc: enthält die schriftliche Ausarbeitung der Seminararbeit und die Präsentation
* libs: enthält die pugixml library
* russ: enthält die Dokumente der zugrundeliegenden Masterarbeit von Fabian Russ
* **src: enthält den Source Code**
* src Russ: enthält den Code von Fabian Russ
* xml enthält die xsd validation files (input.xsd enthält die vollständige Beschreibung des Inputfiles)

## Installation
Folgende Schritte werden vorrausgesetzt:
* Installation von Pugi_XML
* Installation von xerces-c++

Anschließend kann das Tool kompiliert werden. Hierfür wird cmake genutzt. Dazu auf oberster Orderebene folgende Schritte ausführen:

`cmake .`

`make ` 

## Nutzung
Im [bin](bin) Ordner kann die kompilierte Anwendung zusammen mit einem .xml Inputfile gestartet werden. 

` ./roadGeneration <input>.xml`

Das Tool erstellt eine OpenDrive Datei im gleichen Ordner ([bin](bin)). Zur Visualisierung des Steckennetzes eigenet sich der openDrive Viewer von VIRES. 
Wird die OpenDrive Version 1.4 generiert (lässt sich in interface.h umstellen) lassen sich die Strecken ebenfalls mit CarMaker 8.0 darstellen.

## ToDo's
- [x] Verbindung von Segmenten
- [ ] spezifische user-basierte Markierungen sind nicht implementiert
- [ ] automatisch generierte Haltelinien/Pfeile sind nicht implementiert
- [ ] Handbuch ist noch nicht aktualisiert
- [ ] OpenDrive Viewer visualisiert keine Signale oder Markierungen
- [ ] Unterscheidung zwischen M und A Road: aktuell sind M Roads immer durchgehende Straßen an einer Kreuzung und haben eine automatische Spuraufweitung mit Länge(25m). A Roads haben keine zusätzliche Linksabbiegerspur und starten immer im Krezungsmittelpunkt. Diese Definition soll noch verändert werden.

## Beschreibung des Input Files
Die genaue Definition ist dem XSD Schema File zu entnehmen. [input.xsd](xml/input.xsd)

Nachfolgend einige Hinweise:

* segments sind: tjunction, xjunction, njunction (nicht implementiert), roundabout oder connectingroad
* jede junction besitzt einen Typ, der die Stuktur der Kreuzung beschreibt (Beispiel an X Kreuzung: 2M [2 durchgehende Straßen], 4A [4 angrenzende Straßen], M2A [1 durchgehende Straße und 2 angrenzende Straßen])
* jede Kreuzung hat einen Typ (beschreibt automatische Linksabbiegerspur, breitere Spurbreite, Geschwindigkeitslimit)

## Kontakt
Bei Fragen zur Nutzung oder Implementierung:
[christian.geller@ika.rwth-aachen.de](mailto:christian.geller@ika.rwth-aachen.de)

