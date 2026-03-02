
#include "ui.h"

static int DefaultOnDraw(struct Button* ptButton, PDispBuff ptDispBuff)
{
	DrawRegion(&ptButton->tRegion, BUTTON_DEFAULT_COLOR);

	SetFontSize(ptButton->iFontSize);
	DrawTextInRegionCentral(ptButton->name, &ptButton->tRegion, BUTTON_TEXT_COLOR);
	FlushDisplayRegion(&ptButton->tRegion, ptDispBuff);
	return 0;
}

static int DefaultOnPressed(struct Button* ptButton, PDispBuff ptDispBuff, PInputEvent ptInputEvent)
{
	unsigned int dwColor= BUTTON_DEFAULT_COLOR;
	unsigned int textColor = BUTTON_TEXT_COLOR;
	ptButton->status = !ptButton->status;
	if (ptButton->status)
	{
		dwColor = BUTTON_PRESSED_COLOR;
		textColor = BUTTON_TEXTED_COLOR;
	}
	DrawRegion(&ptButton->tRegion, dwColor);
	DrawTextInRegionCentral(ptButton->name, &ptButton->tRegion, textColor);
	FlushDisplayRegion(&ptButton->tRegion, ptDispBuff);
	return 0;
}

void InitButton(PButton ptButton, char* name, PRegion ptRegion, ONDRAW_FUNC OnDraw, ONPRESSED_FUNC OnPressed) {
	ptButton->status = 0;
	ptButton->name = name;
	if (ptRegion)
		ptButton->tRegion = *ptRegion;
	ptButton->OnDraw = OnDraw? OnDraw:DefaultOnDraw;
	ptButton->OnPressed = OnPressed ? OnPressed : DefaultOnPressed;
}


