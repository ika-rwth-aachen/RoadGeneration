# roadGeneration

# Seminararbeit Christian Geller 

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
Folgende Schritte werden im vorrausgesetzt:
* Installation von Pugi_XML
* Installation von xerces-c++

Im folgenden kann das Tool gebaut werden:

`cmake .`

`make ` 

## Anwendung
Im /bin Ordner kann die Executable zusammen mit einem .xml input file gestartet werden:

` ./roadGeneration all.xml`

Dies erstellt eine OpenDrive Datei im gleichen Ordner. Zur Visualisierung eigenet sich der openDrive Viewer. 
Wird die OpenDrive Version 1.4 generiert (lässt sich in interface.h umstellen) lassen sich die Strecken ebenfalls mit CarMaker 8.0 darstellen.

## ToDo's
* spezifische user-basierte Markierungen sind nicht implementiert
* automatisch generierte Haltelinien/Pfeile sind nicht implementiert
* Handbuch ist noch nicht aktualisiert
* OpenDrive Viewer visualisiert keine Signale oder Markierungen
* Unterscheidung zwischen M und A Road: aktuell sind M Roads immer durchgehende Straßen an einer Kreuzung und haben eine automatische Spuraufweitung mit Länge(25m). A Roads haben keine zusätzliche Linksabbiegerspur und starten immer im Krezungsmittelpunkt. Diese Definition soll noch verändert werden.