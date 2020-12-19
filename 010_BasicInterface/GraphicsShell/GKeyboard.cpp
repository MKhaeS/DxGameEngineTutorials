#include "GraphicsShell.h"


void GraphicsShell::KeyDown ( char key ) {
    switch ( key ) {
    case 'W':
        wDown = true;
        break;
    case 'S':
        sDown = true;
        break;
    case 'A':
        aDown = true;
        break;
    case 'D':
        dDown = true;
        break;
    case 'E':
        eDown = true;
        break;
    case 'Q':
        qDown = true;
    break;
    default:
        break;
    }
}

void GraphicsShell::KeyUp ( char key ) {
    switch ( key ) {
    case 'W':
        wDown = false;
        break;
    case 'S':
        sDown = false;
        break;
    case 'A':
        aDown = false;
        break;
    case 'D':
        dDown = false;
        break;
    case 'E':
        eDown = false;
        break;
    case 'Q':
        qDown = false;
        break;
    default:
        break;
    }
}
