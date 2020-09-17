/*

MIT License

Copyright (c) 2020, R. Gregor Weiß, Benjamin Ries

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE
*/

#include "Argument.h"

/*
 * specialization for TP=std:string
 *
 */
template<>
void Argument<std::string>::treat_default_sval() {
    std::string default_sval = this->data_[0];
    std::vector<std::string> snippets;

    size_t found = default_sval.find(" ");
    while (found != std::string::npos) {
        snippets.push_back(default_sval.substr(0, found));
        default_sval = default_sval.substr(found + 1);
        found = default_sval.find(" ");
    }
    snippets.push_back(default_sval);

    this->nargs_ = snippets.size();
    delete[] this->data_;
    this->data_ = new std::string[this->nargs_];
    for (int i = 0; i < std::max<int>(this->nargs_, 1); i++) {
        this->data_[i] = snippets[i];
    } /* i */
}

/*
 * \brief specialization for string
 *
 */
template<>
int Argument<std::string>::parse(int argc, char *argv[]) {
    bool custom(false);
    if (this->pos_ == 0) {
        std::stringstream inss(std::stringstream::in | std::stringstream::out);

        for (int i = 0; i < argc; i++) {
            std::string arg;
            inss.clear();
            inss << argv[i];
            inss >> arg;
            if (this->is(arg)) {
                custom = true;
                if (this->nargs_ == -1) { count_nargs_(i, argc, argv); }
                /* assert that option argument exists */
                assert(i + this->nargs_ < argc);
                for (int j = 0; j < this->nargs_; j++) {
                    inss.clear();
                    inss << argv[i + j + 1];
                    inss >> this->data_[j];
                } /* j */
            }
        } /* i */
        if (!custom) { treat_default_sval(); }
    } else if (argc > this->pos_) {
        std::stringstream inss(std::stringstream::in | std::stringstream::out);
        inss << argv[this->pos_];
        inss >> this->data_[0];
    }

    parsed = true;
    return 0;
}


/*
 * \brief specialization for bool
 */
template<>
int Argument<bool>::parse(int argc, char *argv[]) {
    if (this->pos_ == 0) {
        std::stringstream inss(std::stringstream::in | std::stringstream::out);

        for (int i = 0; i < argc; i++) {
            std::string arg;
            inss.clear();
            inss << argv[i];
            inss >> arg;
            if (this->is(arg)) {
/*                if (this->nargs_ > 0) {
                    assert(i + this->nargs_ < argc);
                    for (int j = 0; j < this->nargs_; j++) {
                        inss.clear();
                        inss << argv[i + j + 1];
                        inss >> this->data_[j];
                    }
                } else {*/
                this->data_[0] = true;
                //}
            }
        } /* i */
    } else if (argc > this->pos_) {
        std::stringstream inss(std::stringstream::in | std::stringstream::out);
        inss << argv[this->pos_];
        inss >> this->data_[0];
    }
    return 0;
}