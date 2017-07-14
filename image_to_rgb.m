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

    % First 3 numbers are number of rows, number of columns
    % and number of colour components per pixel
    fprintf(outputFileID, '%d %d %d\n', m, n, c);
    
    % Print the RGB values for the entire image
    for i=1:m
        for j=1:n
            for k=1:c
                fprintf(outputFileID, '%d ', RGB(i, j, k));
            end
        end
    end
    
    fclose(outputFileID);
