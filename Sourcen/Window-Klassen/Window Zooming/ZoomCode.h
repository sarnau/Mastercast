#pragma once#include <MacWindows.h>typedef void (*CalcIdealDocumentSizeProcPtr)(WindowPtr theWindow, Rect *idealContentSize);void ZoomTheWindow(WindowPeek theWindow, short zoomState,					CalcIdealDocumentSizeProcPtr calcRoutine);