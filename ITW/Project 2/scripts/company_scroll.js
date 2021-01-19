$(document).ready(function() {
    $(window).scroll( function(){
        $('.hidden').each( function(i){
            var object_bottom = $(this).offset().top + $(this).outerHeight();
            var window_bottom = $(window).scrollTop() + $(window).height();
            if( object_bottom < window_bottom ){
              $(this).animate({'opacity':'1'},500);
            }
        });
    });
});
