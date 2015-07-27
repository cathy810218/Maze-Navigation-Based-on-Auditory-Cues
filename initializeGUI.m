function initializeGUI(MAZE_MATRIX, START_POS)
%Initializes the GUI with the appropriate maze data and user starting
%position

    %import global variables
    global tiles
    global facingDirection
    global upArrowImg
    global rightArrowImg
    global downArrowImg
    global leftArrowImg

    %color maze appropriately based on MAZE_MATRIX
    for row = 1:size(MAZE_MATRIX,1)
        for col = 1:size(MAZE_MATRIX,2)
            if (MAZE_MATRIX(row,col))
                tiles{row,col}.Color = [0.25 0.25 0.25];
            else
                tiles{row,col}.Color = [0.88 0.88 0.88];
            end
        end
    end
    %color starting position
    tiles{START_POS(1), START_POS(2)}.Color = [0 0.8 0.4];
    %show the appropriate image based on starting direction
    switch (facingDirection)
        case 1
            imshow(upArrowImg,'Parent',tiles{START_POS(1),START_POS(2)});
        case 2
            imshow(leftArrowImg,'Parent',tiles{START_POS(1),START_POS(2)});
        case 3
            imshow(rightArrowImg,'Parent',tiles{START_POS(1),START_POS(2)});
        case 0
            imshow(downArrowImg,'Parent',tiles{START_POS(1),START_POS(2)});
    end    
    %update parameter bundle sent to C
    updateSoundParams();
end

