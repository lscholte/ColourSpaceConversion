function image_to_rgb(input_file, output_file)

    if nargin == 0
        error('You must pass in the path to an image file');
    elseif nargin == 1
        % Write to stdout
        outputFileID = 1;
    elseif nargin == 2
        outputFileID = fopen(output_file, 'w');
    else
        error('There are too many arguments');
    end

    % Convert image to an m x n by c array
    RGB = imread(input_file);
    [m, n, c] = size(RGB);
    
    if mod(m, 2) == 1
       RGB(m, :, :) = []; 
    end
    
    if mod(n, 2) == 1
       RGB(:, n, :) = []; 
    end

    % First 3 numbers are number of rows, number of columns
    % and number of colour components per pixel
    fprintf(outputFileID, '%d %d %d\n', m, n, c);

    for i=1:2:m
        for j=1:2:n
            for k=1:c
                fprintf(outputFileID, '%d ', RGB(i, j, k));
            end
            
            for k=1:c
                fprintf(outputFileID, '%d ', RGB(i, j+1, k));
            end
            
            for k=1:c
                fprintf(outputFileID, '%d ', RGB(i+1, j, k));
            end
            
            for k=1:c
               fprintf(outputFileID, '%d ', RGB(i+1, j+1, k));
            end
        end
    end



    
    fclose(outputFileID);
