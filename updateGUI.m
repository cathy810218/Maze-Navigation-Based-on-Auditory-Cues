function updateGUI(prevRow, prevCol)
%Updates the GUI by unmarking the previous tile, updating the current
%tile's image, updating the moveCounter, and checking for maze completion

    %import global variables
    global tiles
    global msg
    global currentRow
    global currentCol
    global destRow
    global destCol
    global gameOver
    global playAgain
    global moveCounter
    global numMoved
    global soundPlayer
    global winPlayer
    global upArrowImg
    global downArrowImg
    global leftArrowImg
    global rightArrowImg
    global facingDirection
    
    %unmark previous tile
    cla(tiles{prevRow,prevCol}, 'reset');
    tiles{prevRow,prevCol}.XTick = [];
    tiles{prevRow,prevCol}.YTick = [];
    tiles{prevRow,prevCol}.Color = [0.88 0.88 0.88];
    
    %mark current tile - check current direction
    switch (facingDirection)
        case 1
            imshow(upArrowImg,'Parent',tiles{currentRow,currentCol});
        case 2
            imshow(leftArrowImg,'Parent',tiles{currentRow,currentCol});
        case 3
            imshow(rightArrowImg,'Parent',tiles{currentRow,currentCol});
        case 0
            imshow(downArrowImg,'Parent',tiles{currentRow,currentCol});
    end
    
    %update number moved
    set(moveCounter,'String',num2str(numMoved));
    
    %display winning message if appropriate, stop music, play winning sound
    %and set game as over
    if (currentRow == destRow && currentCol == destCol)
        set(msg,'String','YOU WON!');
        gameOver = 1;
        set(playAgain,'enable','on');
        stop(soundPlayer);
        play(winPlayer);
    end
end

