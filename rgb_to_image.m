function rgb_to_image(input_file, output_file)

    fid = fopen(input_file);
    
    image_dimensions = fscanf(fid, '%d', 3);
    num_rows = image_dimensions(1);
    num_cols = image_dimensions(2);
    num_components = image_dimensions(3);

    %Read in the zig zag file

    % Read the zigzag RGB data into an list of colours
    zigzag = zeros([num_rows*num_cols num_components]);
    for i=1:(num_rows*num_cols)
        v = fscanf(fid, '%d', 3);
        zigzag(i, :) = v;
    end

    img = zeros([num_rows num_cols num_components]);

    toprow = zeros([num_cols num_components]);
    bottomrow = zeros([num_cols num_components]);
    img_index = 1;
    toprow_index = 1;
    bottomrow_index = 1;
    for i=1:4:(num_rows*num_cols)

        % Reads a 2x2 square of colours from the zigzag RGB data
        element1 = zigzag(i, :);
        element2 = zigzag(i+1, :);
        element3 = zigzag(i+2, :);
        element4 = zigzag(i+3, :);

        % The first 2 elements are on the top row of 2x2 square
        toprow(toprow_index, :) = element1;
        toprow(toprow_index+1, :) = element2;

        % The second 2 elements are on the bottom row of 2x2 square
        bottomrow(bottomrow_index, :) = element3;
        bottomrow(bottomrow_index+1, :) = element4;

        toprow_index = toprow_index + 2;
        bottomrow_index = bottomrow_index + 2;

        % If we have reach the end of a row of data
        if mod(i+3, 2*num_cols) == 0

            for j=1:num_cols
               for k=1:num_components
                  img(img_index, j, k) = toprow(j, k) / 255;
                  img(img_index+1, j, k) = bottomrow(j, k) / 255;
               end
            end

            img_index = img_index + 2;
            toprow_index = 1;
            bottomrow_index = 1;
        end
    end
    
    imwrite(img, output_file);
end