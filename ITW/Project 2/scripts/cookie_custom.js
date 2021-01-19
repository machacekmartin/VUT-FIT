$(document).ready(function(){
  if ($.cookie("set") == null){
    $(".cookies_option").hide();
    $.cookie("set", 1);
    $.cookie("invisible",1);
  }
  else if ($.cookie("set") != null){
    if ($.cookie("visible") != null){
      $(".cookies_option").show();
      $(".fa-caret-down").addClass("fa-caret-up").removeClass("fa-caret-down");
    }
    if ($.cookie("hidden") != null){
      $(".cookies_option").hide();
      $(".fa-caret-up").addClass("fa-caret-down").removeClass("fa-caret-up");
    }
  }
  $(".toggle").click(function(){
    if($(".cookies_option").is(':visible')){
      $(".cookies_option").slideUp();
      $.removeCookie("visible");
      $.cookie("hidden", 1);
      $(".fa-caret-up").addClass("fa-caret-down").removeClass("fa-caret-up");
    }
    if($(".cookies_option").is(':hidden')){
      $(".cookies_option").slideDown();
      $.removeCookie("hidden");
      $.cookie("visible", 1);
      $(".fa-caret-down").addClass("fa-caret-up").removeClass("fa-caret-down");
    }
  });
});
