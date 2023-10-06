# Objects

## Object
Object inehåller en x och y variabel, samt texture_id och hälsa ifall det skulle behövas.
Tanken är att saker som spelaren placerar eller andra entities som spelaren behöver påvärka är object.
Eftersom Journey-Bound kommer var atile baserat så tror jag inte att vi behäver ´floats´ för positionen utan ´shorts´ borde vara bra nog

## DynObject
DynObject inehäller ett simpelt inventory, samt sin egna position, sprite och hälsa.
Tanken är att alla saker som ska flytta på sig, som NPCs, fiender, och spelaren ska används DynObject

### Player
spelar objectet inehäller en DynObject bas, samt en pekare till en array för expanderande inventory, och preliminära utrustnings slots.
