define void @swap(i32 %0, i32 %1){
%3 = alloca i32 , align 4
store i32 %0, i32*  %3
store i32 %1, i32 %0
%4 = load i32,i32* %3
store i32 %4, i32 %1
}

@a = global i32 110, align 4
define i32 @main(){
%1 = alloca i32 , align 4
store i32 111, i32*  %1
%2 = alloca i32 , align 4
store i32 222, i32*  %2
%3 = load i32,i32* %1
%4 = load i32,i32* %2
call void @swap(i32 %3, i32 %4)
ret i32 0
}
