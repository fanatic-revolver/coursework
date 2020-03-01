folder='C:\Users\62616\Desktop\AI\AICW2 CODE\animals';   %copy and replace your folder here and run the code
filepaths = dir(fullfile(folder,'*.jpg'));               %take out all the jpg file in the folder
X=zeros(40000,3000);                                     %intiate X to all zeros
y=zeros(3,3000);                                         %intiate y to all zeros
for i=1:999                                              %class 1
    y(1,i)=1;
end
    for i=1000:1999                                      %class 2
        y(2,i)=1;
    end
        for i=2000:3000                                  %class 3
            y(3,i)=1;
        end
for i = 1 : length(filepaths)
    image = imread(fullfile(folder,filepaths(i).name));%read in the i th picture£»
    grayimage=rgb2gray(image);                         %change the img to grey
    standardimg = imresize(grayimage, [200 200]);      %change size to 200
    reshapedimg = reshape(standardimg,[40000,1]);      %reshape the img
    X(:,i)=reshapedimg;                                %put all the things into one coloum
end
save('data.mat','X','y');