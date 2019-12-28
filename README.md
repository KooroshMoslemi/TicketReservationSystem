# Basic Cinema Ticket Reservation System
This is an assignment project for basic c programming class at AUT. The link to the complete project description can be found [here](https://quera.ir/course/assignments/12969/problems)

# A Simple Guide For Installing Dependencies
In this project I have used two third-party libraries:
1. [RapidJson](https://github.com/Tencent/rapidjson/) which is a json parser/generator for c++
2. [VariadicTable](https://github.com/friedmud/variadic_table) which is a formatted table for printing to console

I am going to assume that you have Visual Studio 2019.(the procedure is almost the same for other IDEs.)

First of all,for using the mentioned libraries you should have a copy of ThirdParty directory in your C drive.(ThirdParty directory is provided in the  repository hierarchy)
Second, you should some how tell your IDE that this directory belongs to your project. In visual studio you can right click on your solution and go to: Properties-> C/C++ -> Additional Include Directories -> type: C:\ThirdParty\include\

<b>Note(relatively optional)</b>: Somewhere in the project I had to access directories so I decided to include filesystem.
  Then I encountered some errors and I had to go to my project's property page -> General -> C++ Language Standard and choose
  <b>Preview - Features from the Latest C++ Working Draft(std:c++ latest)</b>
  
  
  # Extra Details About My Implementation
  In this project I assumed that we have two access types:
  1. Admin
  2. User
  
  Admin can define movies for the cinema and delete them if those movies are not in cinema's showing plan.
  He can also define the essential cinema policies. Here I described some of the system features and different factors that admin can     change:
  
  1. The number of salons of cinema(e.g. 4)
  2. The number of show times that admin wants to have in every SALON each WEEK(e.g. 20 show times for ALL of the salons for this week)
    Therefore he can make plan of his cinema weekly.(The main reason for asking just one factor and considering it for all salons in a       specific week is that in this way fewer inputs are needed so testing is easier!)
  3. Duration of show times in general(e.g. 120 minutes)
  4. Defining exact date and time for each show time in each salon individually
  5. The system is bullet proof against time conflicts ;)
  6. Defining a capacity for each show time individually
  
  I assume admin do these setups weekly. Therefore, I ask for the above factors all at once!
  
  
  User action is very simple. He can only reserve tickets. He cannot reserve show times that are filled.
  Random Quick Sort is applied before showing reservation list to user.
  
  
  
  
  








