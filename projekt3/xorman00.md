# PLR projekt 3 - Přiřazení pořadí preorder vrcholům

## Teória

Obecný výpočet v strome má nasledujúce kroky:
1. Vytvorenie eulerovej cesty.
2. Vytvorenie poľa hodnôt.
3. Spočítanie sumi suffixov nad týmto poľom.
4. Korekcia.

Vytvorenie eulerovej cesty je podrobne vysvetlené [tu](http://example.org). V skratke je dôležité vytvoriť `adjency list`. Tento liste sú hrany priradené vrcholom z ktorých vychádzajú. (TODO)

Vytvorenie poľa hodnôt je jednoduché. Dopredné hrany majú hodnotu 1 a ostatné 0. 

Suma suffixov (TODO)

Potom nasleduje už len korekcia. Výsledné indexy sú len v dopredných hranách. `Weight` jednotlivých hrán je invertovaná hodnota výsledku, preto je výsledok rovný `n - weigth(ID)`. Root ma hodnotu 0.

Počet procesorov nutných pre tento algoritmus vypočítame pomocou rovnice `p=2*n-2`, čo znamená že procesorová náročnosť je **n**.

Časová náročnosť sa dá odvodiť nasledovne:
* Výpočet eulerovej cesty sa dá spočítať v konštantnom čase, lebo každý procesor si dokáže materializovať svoju časť potrebnú pre výpočet. V CREW a lepších architektúrach si tiež vedia poskladať procesory eulerovu cestu z medzivýsledkov v konštantnom čase. Zložitosť je teda **c**.
* Vytvorenie poľa hodnôt je taktiež možné v konštantnom čase, v mnohých prípade za 1. Zložitosť je **c**.
* Suma suffixom trvá obecne log n, čo je sposobené tým, že kazdý cyklus sa skáče o viac prvkov prvkov eulerovej cesty. Zložitosť je teda **log n**.
* Korekciu je možné vykonať v konštantnom čase, zložitosť je teda **c**.
Z týchto úvach nám vyjde že `t(n) = c * c * c * log n` čo je `t(n) = O(log n)`.

Celková cena algoritmu je `c(n) = t(n) * p` čo po dosadení je `c(n) = n * log n`.



## Implementácia
Implementácia sa skladá z 2 častí. Shell skriptu `test.sh`, ktorý je použitý na vybuildovanie a spustenie preorder programu a `pr.cpp`, čo je C++ implementácia pararelného hladania preorder poradia v binárnom strome.

C++ časť programu začína načítaným vstupu. Ak je dĺžka vstupu menšia ako 1, program sa rovno ukončí a vypíše nezmenený vstup na `stdout`. Ak je dĺžka vstupu dlhšia ako 1, pokračuje sa dalej.

Každý procesor v tomto programe reprezentuje 1 hranu v grafe, preto je dôležité vedieť odkiaľ a kam daná hrana ukazuje. To sa zistuje pomocou rovníc `myid/4` a `myid/2 + 1` pre kam a odkial smeruje daná hrana. Ak má procesor nepárne ID, tak to znamená že to nie je dopredná hrana, čo sposobý že sa vymenia miesta kam a odkiaľ hrana ide.

Následne sa započne hladanie [eulerovej cesty](http://example.org). Prvou fázou je vytvorenie tkz. `adjecancy listu`. Každý procesor si v záujme optimalizácie vytvorý len tú časť `adjecancy listu`, ktorú bude potrebovať pre nájdenie eulerovej cesty.

Potom vytvorení `adjecancy listu` nasleduje fáza hladania následníka, ktorého algoritmus je zachytený [tu](http://example.org). Potom máme problém, že každý procesor má len svoju časť eulerovej cesty. To vyriešime pomocou funkcie `MPI_Allgather`, ktorá spôsobí, že každý procesor bude mať celú eulerovu cestu.

Ďalej vypočítame opačnú eulerovu cestu. To je potrebné pre sumu suffixov, kedže potrebujeme vedieť koľko krát bude niekto od daného procesoru niečo žiadať. Tá sa vypočíta tak, že každý procesor pošle svojmu následníkovy v eulerovej ceste svoje ID a potom si pomocou `MPI_Allgather` pošlú svoje časti opačnej eulerovej cesty.

Po týchto krokoch začne hladanie poradia preorder vrcholov. Obecný postup je vysvetlený [tu](https://www.fit.vutbr.cz/study/courses/PDA/private/www/h007.pdf#page=30) a [tu](http://example.org). Pri iplementácií je zaujímavý spôsob akým medzi sebou komunikujú jednotlivé procesory. Každý procesor, ak už nie je na konci eulerovej cesty, najpr pošle požiadavku procesoru od ktorého chce jeho `weight` a ID jeho následníka. V tejto požiadavke je jeho ID a ID jeho predchandu v eulerovej ceste. Potom, ak ešte existuje niekto, kto by chcel od neho informácie, sa uspí a čaká kto s ním naviaže spojenie. Tomu pošle svoju `weight` a ID následníka. Toto trvá až kým procesor z ID 0 neprejde až na koniec eulerovej cesty.

Následne je urobená korekcia. Po nej procesory reprezentujúce dopredné hrany pošlú vrchol kam vedú procesu z ID mieste v preorder ceste kam daný vrchol patrí. Následne procesor z ID 0 zozberá celú preorder cestu pomocou `MPI_Gather` a vypíše ju na `stdout`.

## Komunikačný protokol

## Experimenty
Experimenty boli robené na stromoch z dĺžkou 1 až 20. Pre každú dĺžku stromu bolo vykonaných 100 experimentov, ktorých trvanie bolo spriemerované.

![Výsledky experimentov](assets/plot.png)

## Záver