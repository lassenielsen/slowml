# SlowML introduction
This is the SlowML c++ library for machinelearning.

It containse naive implementations for some (simple) machine learning algorithms,
as well as tools using the library for building models and applying them.

As an example these tools can be used to build digit OCR models using the MNIST
open database, and apply them on the test set.

Current results show about 10% error rate on the test set, which is maginally better than a single layer neural network (12% error rate).

# Dependencies
There are very few dependencies, mostly a recent version of the gcc c++ compiler, and make.

# Building and installing the SlowML library
Simply type 
> make clean<br>
> make config<br>
> make build

to build the library, and type
> sudo make install

to install it on your system.

Now you can build applications on your ystem using the slowml library.
We can start by building the example tools.

# Building and installing the SlowML tools
Go to the tools folder by typing
> cd tools

and type
> make clean<br>
> make config<br>
> make build

to build the tools, and type
> sudo make install

to install them on your system.<br>
Return to the project folder by typing
> cd ..

# Tutorial OCR
You can build multilabel classification models (based on logistic regression and one versus all)
and this tutorial will walk you through building a digit OCR model based on the training set from mnist.

First we enter the *data/ocr* folder by typing
> cd data/ocr

Before we can build the model, we need to extract the mnist training examples to featurevectors arranged in label folders. We have written a program to do this, and to build it, simply type
> make mnist\_extract

This will build the mnist\_extract program, so now we go to the train folder by typing
> cd train

and extract the featurevectors by typing
> ../mnist\_extract train-images-idx3-ubyte train-labels-idx1-ubyte

This will print all the training examples, and store them in label folders.
If yout type
> ls

you will see the original mnist image and label filse, but also the folders *label\_0*, *label\_1*, up to *label\_9*, and each folder will contain the featurevectors of the training examples for that label.

Go out of the train folder by typing
> cd ..

Now, to build the model, we need to use the *slowml\_train* tool.
The *slowml\_train* tool takes some config parameters, specifying how to train the model.
Running *slowml\_train* with the default parameters can be extremely slow depending on the number of training examples, features etc.
In this case it takes about 5 hours (on a standard computer) to build the model with the default parameters.
Therefore we will specify less training iterations to make it finish in about one minute.
The main config parameter is *--gd_repetitions* which specifies how many iterations of gradient descend will be used. The default value is *5000*, but for this example we will use only *15* iterations.

To do this type the following command
> slowml\_train --gd\_repetitions 10 -o baddigits.mod train

This will print the found labels (the folders starting with *label\_* in the train folder), and for each label print one dot for each feature it loads in that label.
Then it will use gradient descend to train the model (with *20* iterations) before it stores the model in the *baddigits.mod* file.

Now you will have two models in the current folder
 * baddigits.mod: You have just build this with 15 repetitions
 * digits.mod: A prebuilt model using 5000 repetitions

Now we will test the two models, but before we do this, we need to extract the featurevectors from the mnist test set.
To do this, go to the test folder by typing
> cd test

and extract the featurevectors like we did for the training examples by typing
> ../mnist\_extract t10k-images-idx3-ubyte t10k-labels-idx1-ubyte

This again prints the features, and stores them in the label folders they belog to.
Now go back to the previous folder by typing
> cd ..

because now it is time to test our models!

We will test our models using the *slowml\_test* tool, and we just need to give it the model (with the --model parameter) to test, and the path to the labeled testset.
Thus we test our own model by typing
> slowml\_test --model baddigits.mod test

This will for each test example print the path of the featurevector file, and if it succeeded or failed, and if it failed it will print the guessed and correct label. Finally it will print the success rate and failure rate, and we can see that our bad digits model only succeeded on 73.32% of the examples.

Now we do the same for the prebuilt (and hopefully better) model, by typing
> slowml\_test --model digits.mod test

This again shows the result for each example, but now the final statistics shows that it succeeded on 89.19% of the examples. This is of course better, but then it did take 5 hours to train the model.

If we wish to inspect some of the examples, we can use the tool *slowml\_printvec* which prints a featurevector in an inacurate but intuitive way.
Simply type
> slowml\_printvec --cols 28 test/label\_6/3520.vec

to see the features (which does somewhat show the image) of that example (the --cols 28 is because the images are 28 pixels wide).

Now type
> cd ../../

to return to the project folder and be proud to have trained and tested an OCR model.
