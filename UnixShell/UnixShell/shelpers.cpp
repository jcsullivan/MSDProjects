#include "shelpers.hpp"

/*
  text handling functions
 */

bool splitOnSymbol(std::vector<std::string>& words, int i, char c){
    if(words[i].size() < 2){ return false; }
    int pos;
    if((pos = words[i].find(c)) != std::string::npos){
        if(pos == 0){
            //starts with symbol
            words.insert(words.begin() + i + 1, words[i].substr(1, words[i].size() -1));
            words[i] = words[i].substr(0,1);
        } else {
            //symbol in middle or end
            words.insert(words.begin() + i + 1, std::string{c});
            std::string after = words[i].substr(pos + 1, words[i].size() - pos - 1);
            if(!after.empty()){
                words.insert(words.begin() + i + 2, after);
            }
            words[i] = words[i].substr(0, pos);
        }
        return true;
    } else {
        return false;
    }
    
}

std::vector<std::string> tokenize(const std::string& s){
    
    std::vector<std::string> ret;
    int pos = 0;
    int space;
    //split on spaces
    while((space = s.find(' ', pos)) != std::string::npos){
        std::string word = s.substr(pos, space - pos);
        if(!word.empty()){
            ret.push_back(word);
        }
        pos = space + 1;
    }
    
    std::string lastWord = s.substr(pos, s.size() - pos);
    if(!lastWord.empty()){
        ret.push_back(lastWord);
    }
    
    for(int i = 0; i < ret.size(); ++i){
        for(auto c : {'&', '<', '>', '|', '$', '='}){
            if(splitOnSymbol(ret, i, c)){
                --i;
                break;
            }
        }
    }
    
    return ret;
    
}


std::ostream& operator<<(std::ostream& outs, const Command& c){
  outs << "** " << c.exec << " argv: ";
  for(const auto& arg : c.argv){ if(arg) {outs << arg << ' ';}}
  outs << "** fds: " << c.fdStdin << ' ' << c.fdStdout << ' ' << (c.background ? "background" : "");
  return outs;
}

//returns an empty vector on error
/*

  You'll need to fill in a few gaps in this function and add appropriate error handling
  at the end.

 */
std::vector<Command> getCommands(const std::vector<std::string>& tokens){
    const int READ = 0;
    const int WRITE = 1;
    
  std::vector<Command> ret(std::count(tokens.begin(), tokens.end(), "|") + 1);  //1 + num |'s commands

  int first = 0;
  int last = std::find(tokens.begin(), tokens.end(), "|") - tokens.begin();
  bool error = false;
  for(int i = 0; i < ret.size(); ++i){
      
	if((tokens[first] == "&") || (tokens[first] == "<") ||
		(tokens[first] == ">") || (tokens[first] == "|")){
	  error = true;
	  break;
	}

	ret[i].exec = tokens[first];
	ret[i].argv.push_back(tokens[first].c_str()); //argv0 = program name
	std::cout << "** Parsed: " << ret[i].exec << std::endl;
	ret[i].fdStdin = READ;
	ret[i].fdStdout = WRITE;
	ret[i].background = false;
	
	for(int j = first + 1; j < last; ++j){
	  if(tokens[j] == ">" || tokens[j] == "<" ){
          if (tokens[j] == "<")
          {
              // Checks for appropriate location of input operator.
              if (i != 0)
              {
                  perror("** Input operator in wrong location.");
                  error = true;
                  break;
              }
              // Builds string for target file and checks existence.
              std::string address = "./" + tokens[j+1];
              ret[i].fdStdin = open(address.c_str(), O_RDONLY);
              if (ret[i].fdStdin < 0)
              {
                  perror("** Error opening file to read.");
                  error = true;
                  break;
              }
          }
          if (tokens[j] == ">")
          {
              // Checks for appropriate location of output operator.
              if( i != ret.size() - 1 )
              {
                  perror("** Output operator in wrong location.");
                  error = true;
                  break;
              }
              // Builds string for target file, creates file, and opens it.  
              std::string address = "./" + tokens[j+1];
              ret[i].fdStdout = creat(address.c_str(), 0666);
              if (ret[i].fdStdout < 0)
              {
                  perror("** Error opening file to write.");
                  error = true;
                  break;
              }
              j++;
          }
	  } else if(tokens[j] == "&"){
          ret[i].background = true;
      } else if(tokens[j] == "$"){
          char* varContent = getenv(tokens[j+1].c_str());
          ret[i].argv.push_back(varContent);
          j++;  
      }
      else {
		//otherwise this is a normal command line argument!
		ret[i].argv.push_back(tokens[j].c_str());
	  }
	  
	}
      
    // NOTE:  Pipes have capacity.
    // It's somewhere between 1000 lines of text and 15000 lines of text.
    // Please do not provide any command more than 1000 lines of code, otherwise it might break.  
	if (i > 0)
    {
        int fileDes[2];
        int pipeResult = pipe(fileDes);
        if (pipeResult < 0)
        {
            perror("** Error creating pipe.");
            error = true;
            break;
        }
        
        ret[i].fdStdin = fileDes[READ];
        ret[i-1].fdStdout = fileDes[WRITE];
	}
	//exec wants argv to have a nullptr at the end!
	ret[i].argv.push_back(nullptr);

	//find the next pipe character
	first = last + 1;
	if(first < tokens.size()){
	  last = std::find(tokens.begin() + first, tokens.end(), "|") - tokens.begin();
	}
  }

  if(error){
      for (Command c : ret)
      {
          close(c.fdStdin);
          close(c.fdStdout);
      }
      
      perror("** Error creating command vector.");
  }
  
  return ret;
}
