default: detect_1000.heavy
detect_1.heavy:
	heavyml_train -gdr 1 -gda 100 -n "#features->[[1000*[all]],[#labels*[all],4*[all]]]" -fnt 4 -o detect_1.heavy train/

detect_100.heavy: detect_1.heavy
	heavyml_train -gdr 99 -gda 0 -d -m detect_1.heavy -fnt 4 -o detect_100.heavy train/

detect_200.heavy: detect_100.heavy
	heavyml_train -gdr 100 -gda 0 -d -m detect_100.heavy -fnt 4 -o detect_200.heavy train/

detect_300.heavy: detect_200.heavy
	heavyml_train -gdr 100 -gda 0 -d -m detect_200.heavy -fnt 4 -o detect_300.heavy train/

detect_400.heavy: detect_300.heavy
	heavyml_train -gdr 100 -gda 0 -d -m detect_300.heavy -fnt 4 -o detect_400.heavy train/

detect_500.heavy: detect_400.heavy
	heavyml_train -gdr 100 -gda 0 -d -m detect_400.heavy -fnt 4 -o detect_500.heavy train/

detect_600.heavy: detect_500.heavy
	heavyml_train -gdr 100 -gda 0 -d -m detect_500.heavy -fnt 4 -o detect_600.heavy train/

detect_700.heavy: detect_600.heavy
	heavyml_train -gdr 100 -gda 0 -d -m detect_600.heavy -fnt 4 -o detect_700.heavy train/

detect_800.heavy: detect_700.heavy
	heavyml_train -gdr 100 -gda 0 -d -m detect_700.heavy -fnt 4 -o detect_800.heavy train/

detect_900.heavy: detect_800.heavy
	heavyml_train -gdr 100 -gda 0 -d -m detect_800.heavy -fnt 4 -o detect_900.heavy train/

detect_1000.heavy: detect_900.heavy
	heavyml_train -gdr 100 -gda 0 -d -m detect_900.heavy -fnt 4 -o detect_1000.heavy train/
