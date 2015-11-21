<?php
$classes = [];
$ext = new ReflectionExtension('taglib');
foreach($ext->getClasses() as $class) {
    $className = $class->getName();
    $methods = [];
    foreach($class->getMethods() as $method) {
        if($method->isPublic() && !$method->isStatic()) {
            $methods[$method->getName()] = [["params"=>[],"result"=>null]];
        }
    }
    $classes[$className] = $methods;
}
echo json_encode($classes,JSON_PRETTY_PRINT);
