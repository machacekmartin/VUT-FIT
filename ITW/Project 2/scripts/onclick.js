$(document).ready(function(){
  $('.more').click(function() {
    $(this).toggleClass('fa-caret-down').toggleClass('fa-caret-up');
    $(this).siblings('.text').toggleClass("clicked");
  });
});
