$(document).on("change", ".price-sorting", function() {
    var value = $(this).val();
    if(value == 'lowest'){
        byLowest();
    }
    if(value == 'highest'){
        byHighest();
    }
    if(value == 'default'){
        byDefault();
    }
});
function byLowest(){
  $(".sorting_list li").sort(sort_list).appendTo('.sorting_list');
  function sort_list(a, b) {
    return ($(b).data('price')) < ($(a).data('price')) ? 1 : -1;
  }
}
function byHighest(){
  $(".sorting_list li").sort(sort_list).appendTo('.sorting_list');
  function sort_list(a, b) {
    return ($(b).data('price')) > ($(a).data('price')) ? 1 : -1;
  }
}
function byDefault(){
  $(".sorting_list li").sort(sort_list).appendTo('.sorting_list');
  function sort_list(a, b) {
    return ($(b).data('default')) < ($(a).data('default')) ? 1 : -1;
  }
}
