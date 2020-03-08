#!/usr/bin/php
<?php
    setlocale(LC_ALL, 'ru_RU.UTF-8');
    class item
    {
        var $name;
        var $type;
        var $parent;
        var $relation;
        var $children;
        
    };
    class item2
    {
        var $itemName;
        var $parent;
        var $children;
    };

    function updateNode($src,$dst, $parent)
    {

        $dst->itemName=$src->name;
        $dst->parent=$parent;
        
        $dst->children=Array();
        foreach($src->children as $ch)
        {
            $dst->children[$ch->name]=new item2;
            updateNode($ch,$dst->children[$ch->name],$src->name);
        }
        
    }
    if($argc != 3)
    {
	echo "Usage $argv[0] input.csv output.json";
	exit;
    }
    $fin=$argv[1];
    $fout=$argv[2];

    $allItems=Array();
    $row = 1;
    if (($handle = fopen($fin, "r")) !== FALSE) {
        while (($data = fgetcsv($handle, 13000, ";",'"')) !== FALSE) {
        $num = count($data);
            $row++;

            for ($c=0; $c < $num; $c++) {
            }
            if($num==4){
                $allItems[$data[0]]=new item;
                if(strlen($data[0]))
                {
                    $allItems[$data[0]]->name=$data[0];
                }
                if(strlen($data[1]))
                {
                    $allItems[$data[0]]->type=$data[1];
                }
                if(strlen($data[2]))
                {
                    $allItems[$data[0]]->parent=$data[2];
                }
                if(strlen($data[3]))
                {
                    $allItems[$data[0]]->relation=$data[3];
                }
                $allItems[$data[0]]->children=Array();

                
                
            }
        }
    }
    fclose($handle);

    /// placing parents
    foreach($allItems as $it)
    {
        if(strlen($it->parent)>0 && array_key_exists($it->parent,$allItems))
        {
            $allItems[$it->parent]->children[$it->name]=$it;
        }
    }
    
    /// adding relations
    foreach($allItems as $it)
    {

        if(strlen($it->relation)>0 
         && $it->type=="Прямые компоненты"
            )
        {
            if(array_key_exists($it->relation,$allItems))
            {
                $r=$allItems[$it->relation];
                foreach($r->children as $c)
                {
                    $it->children[$c->name]=$c;
                }

            }
        }
    }
    

    /// converting type 'item' to type 'item2' and setting correct 'parent' field according tree. Recursive calls over tree
    $total=new item2;
    updateNode($allItems["Total"],$total,null);


    $dd=json_encode($total,JSON_PRETTY_PRINT|JSON_UNESCAPED_UNICODE);

    if (($handle = fopen($fout, "w")) !== FALSE) {
        fwrite($handle,$dd);
    }

?>