// ... (верхняя часть файла без изменений до STATE_OPTIONS) ...

    else if (g_state == STATE_OPTIONS) {
        if (g_prevState == STATE_MENU) {
            SolidBrush bg(Color(255, 255, 255, 255)); 
            graphics.FillRectangle(&bg, 0, 0, 800, 600);
        }

        SolidBrush darkOverlay(Color(150, 0, 0, 0));
        graphics.FillRectangle(&darkOverlay, 0, 0, 800, 600);

        RECT modalRect = { 200, 100, 600, 500 };
        SolidBrush modalBg(Color(255, 240, 240, 240));
        graphics.FillRectangle(&modalBg, modalRect.left, modalRect.top, 400, 400);
        
        Pen borderPen(Color(255, 0, 0, 0), 3);
        graphics.DrawRectangle(&borderPen, modalRect.left, modalRect.top, 400, 400);

        RECT titleRect = { 200, 120, 600, 170 };
        DrawTextCustom(hdc, L"OPTIONS", titleRect, 32, Color(255, 0, 0, 0));

        RECT musicRect = { 250, 220, 400, 250 };
        DrawTextCustom(hdc, L"Music", musicRect, 20, Color(255, 0, 0, 0), false);
        
        RECT musicBar = { 400, 225, 550, 245 };
        SolidBrush barBg(Color(255, 150, 150, 150));
        graphics.FillRectangle(&barBg, musicBar.left, musicBar.top, 150, 20);
        SolidBrush barFill(Color(255, 0, 180, 0));
        graphics.FillRectangle(&barFill, musicBar.left, musicBar.top, 80, 20);
        Pen sliderPen(Color(255, 0, 0, 0), 2); 
        graphics.DrawRectangle(&sliderPen, musicBar.left, musicBar.top, 150, 20);

        RECT soundRect = { 250, 300, 400, 330 };
        DrawTextCustom(hdc, L"Sound", soundRect, 20, Color(255, 0, 0, 0), false);
        
        RECT soundBar = { 400, 305, 550, 325 };
        graphics.FillRectangle(&barBg, soundBar.left, soundBar.top, 150, 20);
        graphics.FillRectangle(&barFill, soundBar.left, soundBar.top, 120, 20); 
        graphics.DrawRectangle(&sliderPen, soundBar.left, soundBar.top, 150, 20);

        RECT fpsTextRect = { 250, 370, 400, 400 };
        DrawTextCustom(hdc, L"Show FPS", fpsTextRect, 20, Color(255, 0, 0, 0), false);
        
        RECT fpsBtn = { 400, 375, 480, 415 };
        bool hoverFps = (g_mousePos.x >= fpsBtn.left && g_mousePos.x <= fpsBtn.right && g_mousePos.y >= fpsBtn.top && g_mousePos.y <= fpsBtn.bottom);
        SolidBrush fpsBtnBg(hoverFps ? Color(255, 200, 200, 200) : Color(255, 180, 180, 180));
        graphics.FillRectangle(&fpsBtnBg, fpsBtn.left, fpsBtn.top, 80, 40);
        Pen fpsBorder(Color(255, 0, 0, 0), 2);
        graphics.DrawRectangle(&fpsBorder, fpsBtn.left, fpsBtn.top, 80, 40);

        DrawTextCustom(hdc, g_showFps ? L"ON" : L"OFF", fpsBtn, 16, g_showFps ? Color(255, 0, 180, 0) : Color(255, 0, 0, 0));

        // КНОПКА RESET
        RECT resetTextRect = { 250, 430, 400, 460 };
        DrawTextCustom(hdc, L"Factory Reset", resetTextRect, 20, Color(255, 0, 0, 0), false);
        
        RECT resetBtn = { 400, 430, 480, 470 };
        bool hoverReset = (g_mousePos.x >= resetBtn.left && g_mousePos.x <= resetBtn.right && g_mousePos.y >= resetBtn.top && g_mousePos.y <= resetBtn.bottom);
        SolidBrush resetBtnBg(hoverReset ? Color(255, 255, 100, 100) : Color(255, 200, 150, 150)); // Красноватая кнопка
        graphics.FillRectangle(&resetBtnBg, resetBtn.left, resetBtn.top, 80, 40);
        graphics.DrawRectangle(&fpsBorder, resetBtn.left, resetBtn.top, 80, 40);

        DrawTextCustom(hdc, L"RESET", resetBtn, 16, Color(255, 0, 0, 0));

        RECT backBtn = { 500, 375, 580, 415 };
        bool hoverBack = (g_mousePos.x >= backBtn.left && g_mousePos.x <= backBtn.right && g_mousePos.y >= backBtn.top && g_mousePos.y <= backBtn.bottom);
        
        SolidBrush btnBrush(hoverBack ? Color(255, 200, 200, 200) : Color(255, 180, 180, 180));
        graphics.FillRectangle(&btnBrush, backBtn.left, backBtn.top, 80, 40);
        graphics.DrawRectangle(&borderPen, backBtn.left, backBtn.top, 80, 40);

        DrawTextCustom(hdc, L"BACK", backBtn, 16, Color(255, 0, 0, 0));
    }

// ... (остальная часть файла без изменений) ...
