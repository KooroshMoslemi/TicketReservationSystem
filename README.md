# Basic Cinema Ticket Reservation System
This is an assignment project for basic c programming class at AUT. The link to the complete project description can be found [here](https://quera.ir/course/assignments/12969/problems)

# A Simple Guide For Installing Dependencies
In this project I have used two third-party libraries:
1. [RapidJson](https://github.com/Tencent/rapidjson/) which is a json parser/generator for c++
2. [VariadicTable](https://github.com/friedmud/variadic_table) which is a formatted table for printing to console

I am going to assume that you have Visual Studio 2019.(the procedure is almost the same for other IDEs.)

First of all,for using the mentioned libraries you should have a copy of ThirdParty directory in your C drive.(ThirdParty directory is provided in the  repository hierarchy)
Second, you should some how tell your IDE that this directory belongs to your project. In visual studio you can right click on your solution and go to: Properties-> C/C++ -> Additional Include Directories -> type: C:\ThirdParty\include\

<b>Note(relatively optional)</b>: Some where in the project I had to access directories so I decided to use #include<filesystem>.
  Then I encountered some errors and I had to go to my project's property page -> General -> C++ Language Standard and choose
  <b>Preview - Features from the Latest C++ Working Draft(std:c++ latest)</b>
  








