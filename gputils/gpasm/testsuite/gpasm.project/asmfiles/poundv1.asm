 processor 16c84

number        equ 1

temp_1_suffix equ 1
temp_2_suffix equ 2
temp_3        equ 3
temp_4        equ 4

  fill temp_#v(number)_suffix, 5
  fill temp_#v( (  number + 1 ) )_suffix, 5
  movlw temp_#v( (  number  + 2 ) )
  movlw temp_#v( 2 * 2 )

label_#v(number) movlw 1
label_#v( 5 * 4) movlw 1

craig_#v(number) equ 6

label_#v( ( number ) )_moretext movlw craig_#v(number)

  goto label_1
  goto label_20
  goto label_1_moretext

  end


