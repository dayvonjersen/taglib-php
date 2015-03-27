<?php
$ext = new ReflectionExtension('taglib');
foreach($ext->getClasses() as $class)
{
    if($class->isAbstract())
        echo 'abstract ';
    if($class->isFinal())
        echo 'final ';
    echo 'class ', $class->getName(), "\n";

    foreach($class->getConstants() as $const => $value)
    {
        echo $class->getName(), '::', $const, ' = ', $value, "\n";
    }

    foreach($class->getProperties() as $prop)
    {
        if(!$prop->isDefault())
            continue;

        if($prop->isPrivate())
            echo 'private ';
        if($prop->isProtected())
            echo 'protected ';
        if($prop->isPublic())
            echo 'public ';
        if($prop->isStatic())
            echo 'static ';
        echo '$',$prop->getName(),"\n",$prop->getDocComment(),"\n";
    }

    foreach($class->getMethods() as $meth)
    {
        if($meth->isPrivate())
            echo 'private ';
        if($meth->isProtected())
            echo 'protected ';
        if($meth->isPublic())
            echo 'public ';
        if($meth->isStatic())
            echo 'static ';
        echo $meth->getName(), '( ';
        if($meth->getNumberOfParameters())
        {
            $params = [];
            foreach($meth->getParameters() as $param)
            {
                $val = '';
                if($param->isArray())
                    $val .= 'array ';
                if($param->isCallable())
                    $val .= 'callback ';
                $val .= '$'.$param->getName().' ';
                if($param->isDefaultValueAvailable())
                    $val .= '= '.var_export($param->getDefaultValue(),true).' ';
                elseif($param->isOptional())
                    $val .= '(optional) ';
                $params[] = $val;
            }
            echo implode(', ', $params);
        } else {
            echo 'void';
        }
        echo ' )',"\n",$meth->getDocComment(),"\n";
    }
}
