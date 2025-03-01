Wikidata ist ein ausgezeichneter Anwendungsfall für zelph.
Die Einträge von Wikidata sind über Relationen untereinander vernetzt.
Diese Verknüpfungen unterliegen logischen Einschränkungen.
Wenn zum Beispiel die Klasse `A` das Gegenteil der Klasse `B` ist (Beispiel [successor](tree/Q106110771.md) und [predecessor](tree/Q106110777.md), dann kann es kein `X` geben,
das zu beiden Klassen gehört (wie zum Beispiel [replacing entity](tree/Q45025415.md))

Ausserdem können Schlussfolgerungen getroffen werden.
Wenn zum Beispiel X in Relation zu Y steht und Y in derselben Relation zu Z (Beispiel X=[Canada](Q16.md) und Y=[American continent](tree/Q828.md), Z=[Earth's surface](Q1349417.md), Relation=[is part of](tree/P361.md)),  
und ausserdem die Relation eine [transitive relation](tree/Q64861.md) ist, dann folgt daraus, dass X auch in derselben Relation zu Z steht, also im Beispiel [Canada](Q16.md) [*is part of*](P361.md) [Earth's surface](Q1349417.md).

Die Daten in Form von JSON sind ca. 1.4 TB und beinhalten über 113 Mio. Einträge.
Nach dem Einlesen durch zelph belegt zelph nur 9.2 GB.
Dies wird durch eine auf Bitebene optimierte [Datenstruktur](https://github.com/acrion/zelph/blob/main/src/lib/network.hpp) im Speicher ermöglicht. Für Wikidata werden somit im Durchschnitt nur 80 Byte pro Item benötigt (inklusive der Vernetzungen),
wobei nur die englischen Namen der Items sowie deren wikidata-IDs abgelegt werden.
Einem Item in zelph können Namen in beliebig vielen Sprachen zugewiesen werden, wobei wikidata-IDs gleichberechtigt zu den anderen Sprachen gehandhabt werden (Name der Sprache: "wikidata").
Es gibt ausserdem eine Sprache "zelph", die standardmässig in zelph-Skripten genutzt wird, aber auch dies ist variabel konfigurierbar (über das Kommando `.lang`). 
Das Durchführen sämtlicher anwendbarer Regeln, die aus wikidata.zph hervorgehen, dauert für 4 Durchläufe 2.5 Stunden auf einem Intel Core i9-12900T.
Dabei muss berücksichtigt werden, dass das aktuelle Verfahren noch kein multi threading nutzt.
Jeder Durchlauf kann stark unterschiedliche Zeit benötigen, weil dies von der Zahl der anwendbaren Regeln abhängt.
Nach 4 Durchläufen ist der überwiegende Teil der Inferenz-Möglichkeiten (die neue Facts und Contradictions erzeugen) in den wikidata-Daten ausgeschöpft.

Auf dieser Website veröffentliche ich einen Baum von Seiten. Jede Seite hat eine Entsprechung in Wikidata und ist im Titel verlinkt.
Auf jeder Seite gibt es maximal zwei Abschnitte "Deductions" und "Contradictions". Dies sind Deducations bzw. Contradictions, die in Zusammenhang mit dem Item stehen, das auf der Seite genannt wird.
Zum Beispiel:

- [natural physical object](tree/Q16686022.md) [is for example](tree/Q21514624.md) [Universe](tree/Q1.md) ⇐ ([Universe](tree/Q1.md) [*is a*](tree/P31.md) [natural physical object](tree/Q16686022.md)), ([*is a*](tree/P31.md) [*is inverse of*](tree/P1696.md) [is for example](tree/Q21514624.md))

Der Teil vor dem Zeichen "⇐" ist die Schlussfolgerung. Sie ergibt sich aufgrund unten stehender Regeln aus den Fakten, die hinter dem Zeichen "⇐" stehen.
Im Beispiel wurde diese Regeln angewendet:

```
R is inverse of S,  X R Y    => Y S X   
```

Mit
  - `X`: Universe
  - `Y`: natural physical object
  - `R`: is a
  - `S`: is for example

"is a" ist der Name eines Items in der Sprache "zelph", wie er in unten stehendem Skript festgelegt wird (er ist also nicht hartkodiert).
Der zugehörige englische Name ist "instance of", der zugehörige wikidata-Name ist "P31", siehe https://www.wikidata.org/wiki/Property:P31.

Die Sprache "zelph" wird durch das Kommando `.lang zelph` am Anfang des Skripts festgelegt.



```
.lang zelph

.name !                wikidata Q363948
.name ~                zelph "is a"
.name ~                wikidata P31
.name "is subclass of" wikidata P279
.name =>               wikidata Q7881229
.name ->               wikidata Q18616576
.name "is part of"     wikidata P361
.name "has part"       wikidata P527
.name "is opposite of" wikidata P461
.name "is inverse of"  wikidata P1696
.name "has quality"    wikidata P1552
.name "is for example" wikidata Q21514624
.name "transitive relation" wikidata Q18647515

#has quality    is a ->
#has part       is a ->
#is opposite of is a ->
#is inverse of  is a ->
#is for example is a ->
#"is subclass of" "is a" ->

# The following facts are part of wikidata:
#"is subclass of" is a  transitive relation
#"has part"       is a transitive relation
#"is part of"     is inverse of "has part"

# The following facts are not part of wikidata:
"has quality" is a transitive relation

# The following (quite obvious) fact is not part of wikidata. Wikidata only includes the fact "is subclass of" "subject item of this property" "is for example"
"is for example"  is inverse of "is a"

R is a transitive relation, X R Y, Y R Z => X R Z
P is a transitive relation, P is inverse of Q => Q is a transitive relation
X is a K, K is subclass of U => X is a U

X has quality E,   E is a K            => X has quality K
X has quality E,   E is subclass of K  => X has quality K
K has quality E,   X is a K            => X has quality E
K has quality E,   X is subclass of K  => X has quality E
X has part P,      P is a K            => X has part K
K has part P,      X is subclass of K  => X has part P

X is opposite of Y, X is a K           => Y is a K
X is opposite of Y, X is subclass of K => Y is subclass of K
X is inverse of Y,  X is a K           => Y is a K
X is inverse of Y,  X is subclass of K => Y is subclass of K
X is opposite of Y           => Y is opposite of X
X is inverse of Y            => Y is inverse of X
R is opposite of S, X R Y    => Y S X
R is inverse of S,  X R Y    => Y S X

X is opposite of Y, A has quality X, A has quality Y => !
X is inverse of Y,  A has quality X, A has quality Y => !
X is opposite of Y, A has part X, A has part Y => !
X is inverse of Y,  A has part X, A has part Y => !
X is opposite of Y, A is a X,           A is a Y => !
X is opposite of Y, A is subclass of X, A is subclass of Y => !
X is inverse of Y,  A is a X,           A is a Y => !
X is inverse of Y,  A is subclass of X, A is subclass of Y => !

X has quality E, X is a E           => !
X has quality E, X is subclass of E => !
X has quality E, E is a X           => !
X has quality E, E is subclass of X => !
X has quality E, E has part X       => !

X has part E, X is a E           => !
X has part E, X is subclass of E => !
X has part E, E is a X           => !
X has part E, E is subclass of X => !
#(matter  has part  chemical substance), (chemical substance  is subclass of  matter)

# The following contradiction requires that X cannot be at the same time an instance and a subclass:
X is a A, X is subclass of B => !
# But this doesn't seem to make sense for wikidata, because there are X that are at the same time subclasses and instances, e. g. "black":
# ! <= (Q23445  is a  Q1075), (Q23445  is subclass of  Q40890790)
# ! <= (black  is a  color), (black  is subclass of  absorbed or reflected object light)

A is a B, B is a A                     => !
A is subclass of B, B is subclass of A => !
A is a B, B is subclass of A           => !

#.wikidata Q23445-black.json
#.wikidata /Users/stefan/_excludedFromBackup/latest-all.json 2
#.wikidata Y:/_excludedFromBackup/latest-all.json 2
#.wikidata /mnt/hgfs/_excludedFromBackup/latest-all.json 2
#.wikidata latest-all.json 2
```

Hier beispielhaft eine der vielen Auswirkungen, die dieses Skript auf die Art und Weise hat, wie zelph die wikidata-Daten verarbeitet.
Wikidata macht eine granulare Unterscheidung zwischen "is a"-Relationen. Es gibt folgende Arten:

 - [instance of](tree/P31)
 - [subclass of](tree/P279)
 - [facet of](tree/P1269)

zelph is so flexibel designt, dass es dies problemlos abbilden kann. Obiges Skript soll allgemeingültig auf den wikidata-Daten
funktionieren. Das bedeutet nicht, dass es keine Widersprüche in wikidata finden darf (denn sie könnten unbeabsichtigt sein).
Der Anspruch ist, dass es in exaktem Einklang mit den [Wikidata usage instructions](https://www.wikidata.org/wiki/Property:P2559) ist, die auf vielen Seiten angegeben sind.

Im konkreten Beispiel der drei obigen "is a" Relationen fällt auf, dass wikidata nur "subclass of" als transitiv auszeichnet,
nicht die anderen beiden. Für "instance of" ergibt das Sinn, denn eine Instanz ist keine Klasse. Für "facet of" ergänzt das Skript
diese Eigenschaft und ergänzt ausserdem Regeln, die dessen auf https://wikidata.org/wiki/Property:P1269 dokumentierte Bedeutung
widerspiegeln: Ist X eine [facet of](tree/P1269) Y, dann erbt X alle Eigenschaften von Y. Daher stehen für diesen Fall folgende Regeln im Skript:

- Ist `X` eine [instance of](tree/P31) `C`, dann muss auch `Y` eine [instance of](tree/P31) `C` sein.
- Ist `X` ist eine [subclass of](tree/P279) `C`, dann muss auch `Y` eine [subclass of](tree/P279) `C` sein.
- Hat `X` [einen Teil](tree/P527) `P`, dann muss auch `Y` [einen Teil](tree/P527) `P` haben.  
- Ist `X` [Teil von](tree/P361) `P`, dann muss auch `Y` [Teil von](tree/P361) `P` sein.
- Hat `X` [eine Characteristic](tree/P1552) `Q`, dann muss auch `Y` [eine Characteristic](tree/P1552) `Q` haben.

Hier die schrittweise Beschreibung einer konkreten Schlussfolgerung, die zelph aufgrund der wikidata-Daten und des Skripts wikidata.zlp trifft:

Die Property [greater than](tree/P5135) ist laut wikidata eine Instanz von [transitive Wikidata property](tree/Q18647515).
Da [transitive Wikidata property](tree/Q18647515) eine [facet of](tree/P1269) [transitive relation](tree/Q64861) ist,
greift die Regel `X is facet of Y, Y is a C => X is a C`. Somit schlussfolgert zelph, dass [greater than](tree/P5135)
auch Instanz von [transitive relation](tree/Q64861) ist. Wenn wir 

[logical consequence](tree/Q374182)

[transitive Wikidata property](tree/Q18647515)
"facet of"
[transitive relation](tree/Q64861)
"subclass of"
[quasitransitive relation](tree/Q7269539)
"subclass of"
[binary relation](tree/Q130901)


im Abschnitt [Semantic Analysis of Wikidata]()

Allgemeines:

Regeln sind im gleichen semantischen Netzwerk kodiert, wie die Fakten.
Dieser innovative Ansatz ermöglicht ein enges Zusammenspiel zwischen der Faktenbasis und den Regeln.
Eine Regel ist nur ein Spezialfall eines Faktums, das die Relation `=>` nutzt.
Im Falle der [Anwendung von zelph auf Wikidata-Daten](wikidata.md) entspricht diese Relation der [logical consequence](tree/Q374182).

Fakten, die logische Aussagen
über Relationen machen, sind ein mächtiger Anwendungsfall von zelph. Beispiel:

```
R is transitive, X R Y, Y R Z => X R Z
```

wikidata kennt selbst Aussagen über Relationen, inklusive der [Transitivität](tree/Q18647515.md).
Dadurch 

Ein Beispiel-Skript:

```
X is a Y  => X ~ Y
X is an Y => X is a Y

is               is a ->
has part         is a ->
"is opposite of" is a ->

"is attribute of" is opposite of "is"
"is part of"      is opposite of "has part"
"is for example"  is opposite of "is a"

"has part"      is transitive
"has attribute" is transitive
~               is transitive

R is transitive, X R Y, Y R Z => X R Z
X is E, E is a K  => X is K
X has part P,      P is a K  => X has part K
K is E, X is a K  => X is E
K has part P,      X is a K  => X has part P
X is opposite of Y, X is a K => Y is a K
X is opposite of Y           => Y is opposite of X
R is opposite of S, X R Y    => Y S X

X is opposite of Y, A is X, A is Y => !
X is opposite of Y, A has part X, A has part Y => !
X is opposite of Y, A is a X, A is a Y => !
X is E, X is a E     => !
X is E, E is a X     => !
X is E, E has part X => !

generates is a ->
needs is a ->

"is needed by" is opposite of needs
"is generated by" is opposite of generates

is needed by is opposite of needs
is generated by  is opposite of generates

X generates energy  => X is an "energy source"
A is hot => A generates heat
A generates oxygen  => A is alive


chimpanzee is an ape
ape is alive

chimpanzee has part hand
hand has part finger

"green mint" is an mint

"water mint" is a mint

peppermint is an mint

mint is a lamiacea

catnip is a lamiacea

"green mint" is sweet

"is ancestor of" is transitive
peter is ancestor of paul
paul is ancestor of pius
A is ancestor of pius
```

Die letzte Codezeile `A is ancestor of pius` wird als Frage interpretiert. Das Ergebnis sieht man in der ersten
Zeile unten stehender Antwort, gefolgt von weiteren Schlussfolgerungen:

```
Answer: «paul» «is ancestor of» «pius»
«catnip» «~» «lamiacea» ⇐ «catnip» «is a» «lamiacea»
«needs» «~» «->» ⇐ «needs» «is a» «->»
«water mint» «~» «mint» ⇐ «water mint» «is a» «mint»
«mint» «~» «lamiacea» ⇐ «mint» «is a» «lamiacea»
«chimpanzee» «has part» «finger» ⇐ («hand» «has part» «finger»), («chimpanzee» «has part» «hand»), («has part» «is» «transitive»)
«peter» «is ancestor of» «pius» ⇐ («paul» «is ancestor of» «pius»), («peter» «is ancestor of» «paul»), («is ancestor of» «is» «transitive»)
«water mint» «~» «lamiacea» ⇐ («mint» «~» «lamiacea»), («water mint» «~» «mint»), («~» «is» «transitive»)
«peppermint» «is a» «mint» ⇐ «peppermint» «is an» «mint»
«chimpanzee» «is a» «ape» ⇐ «chimpanzee» «is an» «ape»
«green mint» «is a» «mint» ⇐ «green mint» «is an» «mint»
«chimpanzee» «is» «alive» ⇐ («chimpanzee» «is a» «ape»), («ape» «is» «alive»)
«generates» «is opposite of» «is generated by» ⇐ «is generated by» «is opposite of» «generates»
«has part» «is opposite of» «is part of» ⇐ «is part of» «is opposite of» «has part»
«is a» «is opposite of» «is for example» ⇐ «is for example» «is opposite of» «is a»
«is» «is opposite of» «is attribute of» ⇐ «is attribute of» «is opposite of» «is»
«needs» «is opposite of» «is needed by» ⇐ «is needed by» «is opposite of» «needs»
«finger» «is part of» «hand» ⇐ («hand» «has part» «finger»), («has part» «is opposite of» «is part of»)
«hand» «is part of» «chimpanzee» ⇐ («chimpanzee» «has part» «hand»), («has part» «is opposite of» «is part of»)
«finger» «is part of» «chimpanzee» ⇐ («chimpanzee» «has part» «finger»), («has part» «is opposite of» «is part of»)
«sweet» «is attribute of» «green mint» ⇐ («green mint» «is» «sweet»), («is» «is opposite of» «is attribute of»)
«alive» «is attribute of» «ape» ⇐ («ape» «is» «alive»), («is» «is opposite of» «is attribute of»)
«transitive» «is attribute of» «is ancestor of» ⇐ («is ancestor of» «is» «transitive»), («is» «is opposite of» «is attribute of»)
«alive» «is attribute of» «chimpanzee» ⇐ («chimpanzee» «is» «alive»), («is» «is opposite of» «is attribute of»)
«transitive» «is attribute of» «has part» ⇐ («has part» «is» «transitive»), («is» «is opposite of» «is attribute of»)
«transitive» «is attribute of» «~» ⇐ («~» «is» «transitive»), («is» «is opposite of» «is attribute of»)
«transitive» «is attribute of» «has attribute» ⇐ («has attribute» «is» «transitive»), («is» «is opposite of» «is attribute of»)
«mint» «is for example» «green mint» ⇐ («green mint» «is a» «mint»), («is a» «is opposite of» «is for example»)
«lamiacea» «is for example» «catnip» ⇐ («catnip» «is a» «lamiacea»), («is a» «is opposite of» «is for example»)
«->» «is for example» «needs» ⇐ («needs» «is a» «->»), («is a» «is opposite of» «is for example»)
«mint» «is for example» «water mint» ⇐ («water mint» «is a» «mint»), («is a» «is opposite of» «is for example»)
«->» «is for example» «is» ⇐ («is» «is a» «->»), («is a» «is opposite of» «is for example»)
«->» «is for example» «has part» ⇐ («has part» «is a» «->»), («is a» «is opposite of» «is for example»)
«ape» «is for example» «chimpanzee» ⇐ («chimpanzee» «is a» «ape»), («is a» «is opposite of» «is for example»)
«lamiacea» «is for example» «mint» ⇐ («mint» «is a» «lamiacea»), («is a» «is opposite of» «is for example»)
«->» «is for example» «is opposite of» ⇐ («is opposite of» «is a» «->»), («is a» «is opposite of» «is for example»)
«->» «is for example» «generates» ⇐ («generates» «is a» «->»), («is a» «is opposite of» «is for example»)
«mint» «is for example» «peppermint» ⇐ («peppermint» «is a» «mint»), («is a» «is opposite of» «is for example»)
«green mint» «~» «mint» ⇐ «green mint» «is a» «mint»
«chimpanzee» «~» «ape» ⇐ «chimpanzee» «is a» «ape»
«peppermint» «~» «mint» ⇐ «peppermint» «is a» «mint»
«peppermint» «~» «lamiacea» ⇐ («mint» «~» «lamiacea»), («peppermint» «~» «mint»), («~» «is» «transitive»)
«green mint» «~» «lamiacea» ⇐ («mint» «~» «lamiacea»), («green mint» «~» «mint»), («~» «is» «transitive»)
«is needed by» «is a» «->» ⇐ («needs» «is a» «->»), («needs» «is opposite of» «is needed by»)
«is attribute of» «is a» «->» ⇐ («is» «is a» «->»), («is» «is opposite of» «is attribute of»)
«is part of» «is a» «->» ⇐ («has part» «is a» «->»), («has part» «is opposite of» «is part of»)
«is generated by» «is a» «->» ⇐ («generates» «is a» «->»), («generates» «is opposite of» «is generated by»)
«->» «is for example» «is generated by» ⇐ («is generated by» «is a» «->»), («is a» «is opposite of» «is for example»)
«->» «is for example» «is attribute of» ⇐ («is attribute of» «is a» «->»), («is a» «is opposite of» «is for example»)
«->» «is for example» «is needed by» ⇐ («is needed by» «is a» «->»), («is a» «is opposite of» «is for example»)
«->» «is for example» «is part of» ⇐ («is part of» «is a» «->»), («is a» «is opposite of» «is for example»)
«is generated by» «~» «->» ⇐ «is generated by» «is a» «->»
«is needed by» «~» «->» ⇐ «is needed by» «is a» «->»
Ready.
```

 
Ein zweiter besonderer Designaspekt von zelph ist die Abbildung des Relationstyps.
Wenn in einem semantischen Netzwerk A und B durch eine Relation miteinander verbunden
sind (A --> B), dann wird üblicherweise der Pfeil beschriftet, es wird also ein Label vergeben.

``` mermaid
graph TD;
    "white" -- is opposite of --> "black";
```

So ein Relations-Label ist also normalerweise etwas, das sich von Knoten unterscheidet.
In zelph ist das nicht so: Der Typ einer Relation ist einfach ein weiterer Knoten.
Auf zelph-Ebene gibt einen Standard-Relations-Typ `~`, was eine Kategorie-Beziehung
zwischen A und B darstellt. Es gibt zwei Abweichungen von diesem Standard:

- Fakten (wie im Folgenden erläutert)
- Regeln (weiter unten erläutert)

Dies ist eine Metaebene, die für die eigentlichen Fakten, die zelph Netzwerk abbildet, nicht relevant ist.
Denn ein Relationstyp zwischen A und B, der keine zelph-Kategorie-Beziehung ist,
wird in zelph abgebildet, indem ein spezieller Relations-Knoten erzeugt wird.
Beispiel: In einem zelph-Skript kann man schreiben:

```
is opposite of ~ ->
> «is opposite of» «~» «->»
white is opposite of black
> «white» «is opposite of» «black»
```

Hinter `>` sieht man jeweils die Antworten von zelph auf die beiden Eingabezeilen. Die erste Zeile `is opposite of ~ ->` kann
auch weggelassen werden, da sich zelph aufgrund der Syntax erschliesst, dass es sich um eine Relation (`->`) handeln muss:

```
white "is opposite of" black
> «white» «is opposite of» «black»
```

Im Unterschied zum ersten Beispiel, wo wir "is opposite of" explizit als Relation bekannt geben, muss hier
"is opposite of" in Anführungszeichen geschrieben werden, da es Leerzeichen enthält. Sobald der Name dieses
Relations-Knotens bekannt ist, sind keine Anführungszeichen mehr notwendig.

Nun zur internen Abbildung dieser Relationen im zelph-Netzwerk:

Die Zeile `is opposite of ~ ->` (die im zweiten Beispiel intern automatisch ausgeführt wird)
erzeugt zwei neue Knoten im zelph-Netzwerk:

``` mermaid
graph TD;
    "is opposite of ~ ->" <--> "is opposite of";
    "is opposite of ~ ->" <-- "->";
    "is opposite of ~ ->" --> "~";
```

Der Knoten "is opposite of ~ ->" repräsentiert diese spezielle Relation (daher ist er so benannt, wie die Eingabezeile).
Die Relationen zu den anderen Knoten kodieren seine Bedeutung. Dabei gilt für die Richtung der Relation:

| Element        | Relationsrichtung   |
|----------------|---------------------|
| Subjekt        | beidseitig          |
| Objekt         | rückwärts           |
| Relationstyp   | vorwärts            |

Diese Semantik nutzt zelph in mehreren Zusammenhängen, zum Beispiel der Unifikation von Regeln.
Sie ist erforderlich, weil zelph, wie oben erläutert, Relationstypen nicht als Labels an Pfeilen kodiert, sondern
wie gleichberechtigte Knoten. Dies hat den Vorteil, dass Aussagen über Aussagen erleichtert werden, zum Beispiel
die Aussage, dass eine Relation transitiv ist. Dies wird bei der Unifikation von Regeln genutzt.

Die Knoten `->` und `~` sind vordefinierte zelph-Knoten.
`->` repräsentiert die Kategorie aller Relationen. `~` repräsentiert eine Teilmenge dieser Kategorie, nämlich die Kategorie der Kategorie-Relationen.
Jede Relation, die sich von der Standard-Relation `~` unterscheidet (wie "is opposite of") ist über eine `~`-Relation mit `->` verknüpft.
Zum Beispiel sagt die Relation zwischen "is opposite of ~ ->" und "->" aus, dass "is opposite of ~ ->" zur Kategorie der Relationen gehört.

Die zweite Zeile "white is opposite of black" erzeugt drei neue Knoten:

``` mermaid
graph TD;
    "white is opposite of black" <--> "white";
    "white is opposite of black" <-- "black";
    "white is opposite of black" --> "is opposite of";
```

In beiden Beispielen ist der Relationsknoten so benannt, wie die Relation, die durch das Netzwerk umgesetzt wird:
"is opposite of ~ ->" bzw. "white is opposite of black" - was der Nutzer-Eingabe entspricht. Dieser spezielle Knoten
repräsentiert genau diese spezielle Relation. Damit zelph weiss, um welche Relation es geht, ist er mit dieser
verknüpft, zeigt also auf "~" bzw. "is opposite of". Im zweiten Beispiel muss zusätzlich kodiert werden, dass es sich
beim Knoten "is opposite of" um eine Relation handelt. Dies geschieht wiederum durch eine Verknüpfung zu einem Relationsknoten
"is opposite of ~ ->". 

Dieses Design schliesst aus, dass Subjekt und Objekt einer Relation identisch sind.
Hierfür gibt es Beispiele in wikidata, z.B. "South Africa (Q258)" "country (P17)" "South Africa (Q258)".
"South Africa" ist also in wikidata über die Relation (Property) "Country" mit sich selbst verknüpft.
Diese Beispiele in wikidata sind selten und werden beim Import ignoriert und eine entsprechende Warnung erscheint.

# Regeln

Regeln in der Skriptsprache enthalten immer die Zeichen `=>`. Vor dem Zeichen stehen eine
oder mehrere Bedingungen (durch Kommata getrennt), die alle zutreffen müssen (and-Verknüpfung). 
Hinter dem Zeichen steht eine Konsequenz, falls die Regel greift. Alternativ steht durch 
ein `!`, was auf einen Widerspruch hinweist. Wird ein Widerspruch erkannt, während ein fact
eingegeben wird (durch die Skriptsprache, oder beim Import von wikidata-Daten), so wird die
entsprechende Relation (das fact) nicht ins semantische Netzwerk eingetragen. Stattdessen wird ein
Fakt eingetragen, dass diesen Widerspruch beschreibt (wodurch er beim Markdown-Export der Fakten einsehbar ist).

Beispiel für eine Regel:

```
R ~ transitive relation, X R Y, Y R Z => X R Z
> ((Y R Z), (X R Y), (R «~» «transitive relation»)) «=>» (X R Z)
```

Nach dem `>` sieht man wieder die Ausgabe von zelph, die in diesem Fall einfach nur die Eingabe der Regel bestätigt.

Nun geben wir bekannt, dass die Relation `>` (grösser) zur Kategorie (`~`) der transitiven Relationen gehört:

```
> ~ transitive relation
> «>» «~» «transitive relation»
```

Nun geben wir drei Elemente bekannt, für die die `>` Relation gilt: 


```
6 > 5
> «6» «>» «5»
5 > 4
> «5» «>» «4»
«6» «>» «4» ⇐ («5» «>» «4»), («6» «>» «5»), («>» «~» «transitive relation»)
```

Nach der Eingabe von `5 > 4` greift der Unifikations-Mechanismus von zelph und fügt ein
neues Fakt ein: `6 > 4`.

Bei der Anwendung auf Wikidata-Daten werden also nicht nur Fakten geprüft, sondern es finden auch
komplexe Anwendungen von logischen Verknüpfungen statt, die in wikidata selbst kodiert sind.
Das ist möglich, weil zelph für jeden Knoten Namen in mehreren Sprachen ermöglicht. Obige Skripte
sind in der Sprache "zelph" geschrieben, aber aufgrund des Skripts `wikidata.zlp` kann zelph
zuordnen, dass der Knoten "transitive relation" identisch ist mit dem wikidata-Knoten Q64861,
der zum Beispiel mit der Wikidata-Property P5136 ("less than") verknüpft ist. Auf diese Weise
wird geprüft, ob sich aus den Daten von wikidata Widersprüche ergeben und es werden viele
neue Fakten generiert. Dafür muss natürlich `wikidata.zlp` den [Wikidata usage instructions](https://www.wikidata.org/wiki/Property:P2559)
entsprechen - aber das Skript ist nicht Teil von zelph, sondern kann gepflegt und 
ausgebaut werden, was weitere spannende Erkenntnisse nach sich ziehen dürfte.


Der Import von Wikidata-Daten geschieht über das Kommando `.wikidata`, zum Beispiel

```
.wikidata download/wikidata-20250127-all.json
```

Hierbei legt zelph zunächst eine `.index`-Datei im gleichen Verzeichnis an, die zukünftige Ladevorgänge beschleunigt.

Anschliessend kann man den Inferenz-Mechanismus starten. Entweder über

```
.run
```

oder über 

```
.run-md
```

Letzteres setzt ein leeres Verzeichnis `mkdocs/docs/tree` voraus, in dem Markdown-Dateien angelegt werden, die wikidata referenzieren.
