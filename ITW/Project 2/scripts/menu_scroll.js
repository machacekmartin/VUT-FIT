$(document).ready(function(){
  var scrolled = false;
    $(window).scroll(function() {
        if (!scrolled && $(this).scrollTop() >= 150) {
            $("#menu").addClass("scrolled");
            scrolled = true;
        }
        if (scrolled && $(this).scrollTop() < 150) {
            $("#menu").removeClass("scrolled");
            scrolled = false;
        }
    });
});
